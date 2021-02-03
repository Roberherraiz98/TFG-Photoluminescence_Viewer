#include <sstream>
#include "synchronousgrab.h"
#include "ui_synchronousgrab.h"
#include "asynchronousgrab.h"

#include <QFileDialog>
#include <QFile>
#include <thread>
#include <chrono>

#include <iostream>
#include <ctime>

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#include <thread>
#include <iostream>
#include <string>


#define CAMERA_RETARD 2560

using AVT::VmbAPI::FramePtr;
using AVT::VmbAPI::CameraPtrVector;

int retardtimesync = 0;
int pwmdurationtimesync = 0;


enum { NUM_COLORS = 3, };
enum { BIT_DEPTH  = 8, };


void FlashTaskSynchro(){

    if(retardtimesync >= 0){
        delay(CAMERA_RETARD);
    }

    system("sudo chown root:ubuntu /dev/gpiomem");//grants access to GPIO pins
    system("sudo chmod g+rw /dev/gpiomem ");

    wiringPiSetup();
    softPwmCreate(0, 0, 1);//Pin 0, but pin number 11 in Raspberry
    softPwmWrite(0, 1);

    //TIME STAMP
    time_t rawtime;
    int milisec;
    struct tm * timeinfo;
    struct timeval tv;
    char buffer[80];

    gettimeofday(&tv, NULL);

    milisec = lrint(tv.tv_usec/1000.0);
    if (milisec>=1000){
        milisec -=1000;
        tv.tv_sec++;
    }

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d_%m_%Y_%I:%M:%S", timeinfo);

    //printf("%s.%03d", buffer, milisec);
    std::string str(buffer);
    std::cout<< "Flash: " <<str << "." << milisec << std::endl;

    delay(pwmdurationtimesync);
    softPwmWrite(0, 0);

}

void RetardTaskSynchro(){
    if(retardtimesync<0){
        delay(abs(retardtimesync) + CAMERA_RETARD);
    }
    else
        delay(abs(retardtimesync));

}
void RetardFlashTaskSynchro(){//Combines Retard + Flash and it is used when retardtime is < 0
    RetardTaskSynchro();
    FlashTaskSynchro();
}


SynchronousGrab::SynchronousGrab(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , m_pApiController( new ApiController() )
{

    ui.setupUi(this);
    ui.m_LabelStream->setAlignment( Qt::AlignCenter );
    // Connect GUI events with event handlers
    QObject::connect( ui.m_ButtonStartStop, SIGNAL( clicked() ), this, SLOT( OnBnClickedButtonStartstop() ));

    // Start Vimba
    VmbErrorType err = m_pApiController->StartUp();
    Log( "Starting Vimba", err );
    if ( VmbErrorSuccess == err )
    {
        std::string WindowText("Synchronous Grab ");
        this->setWindowTitle( QString::fromStdString(WindowText) );
        // Connect new camera found event with event handler
        QObject::connect( m_pApiController->GetCameraObserver(), SIGNAL( CameraListChangedSignal( int )), this, SLOT( OnCameraListChanged( int )));

        // Initially get all connected cameras
        UpdateCameraListBox();
        std::stringstream strMsg;
        strMsg << "Cameras found..." << m_cameras.size();
        Log( strMsg.str() );
    }
}

SynchronousGrab::~SynchronousGrab()
{
    // Before we close the application we stop Vimba
    m_pApiController->ShutDown();

    delete m_pApiController;
    m_pApiController = NULL;
}

void SynchronousGrab::OnBnClickedButtonStartstop()
{
    AsynchronousGrab a;
    retardtimesync = a.GetRetardTime();
    pwmdurationtimesync = a.GetPWMDuration();

    int nRow = ui.m_ListBoxCameras->currentRow();

    //SHOWING CURRENT PARAMETERS WHILE ACQUIRING IMAGES
    std::stringstream sss;
    sss << "Current retard time: " << retardtimesync << " ms \n";
    sss << "Current flash duration: " << pwmdurationtimesync << " ms \n";
    std::string sstext = sss.str();
    ui.CurrentParametersSynchro->setText(QString::fromStdString(sstext));

    if ( -1 < nRow )
    {
        if (retardtimesync >= 0){

            std::thread t1(FlashTaskSynchro);
            std::thread t2(RetardTaskSynchro);


            t1.detach();//do not wait for t1 to end
            t2.join();//wait for t2 to end until continuing compilation

            SynchronousGrab::SingleImageTaskSynchro(nRow);
        }
        else//retard time < 0
        {

            std::thread t1(RetardFlashTaskSynchro);
            std::thread t2(&SynchronousGrab::SingleImageTaskSynchro, this, nRow);

            t2.join();
            t1.detach();
        }
    }
    else
    {
        Log( "Please select a camera." );
    }

}

//
// This event handler (Qt slot) is triggered through a Qt signal posted by the camera observer
//
// Parameters:
//  [in]    reason          The reason why the callback of the observer was triggered (plug-in, plug-out, ...)
//
void SynchronousGrab::OnCameraListChanged( int reason )
{
    bool bUpdateList = false;

    // We only react on new cameras being found and known cameras being unplugged
    if ( AVT::VmbAPI::UpdateTriggerPluggedIn == reason )
    {
        Log( "Camera list changed. A new camera was discovered by Vimba." );
        bUpdateList = true;
    }
    else if ( AVT::VmbAPI::UpdateTriggerPluggedOut == reason )
    {
        Log( "Camera list changed. A camera was disconnected from Vimba." );
        bUpdateList = true;
    }

    if ( true == bUpdateList )
    {
        UpdateCameraListBox();
    }

    ui.m_ButtonStartStop->setEnabled( 0 < m_cameras.size() || m_bIsStreaming );
}

//
// Copies the content of a byte buffer to a Qt image with respect to the image's alignment
//
// Parameters:
//  [in]    pInbuffer       The byte buffer as received from the cam
//  [out]   OutImage        The filled Qt image
//
void SynchronousGrab::CopyToImage( VmbUchar_t *pInBuffer, QImage &OutImage )
{
    VmbUchar_t *    pCursor = OutImage.bits();
    int             nHeight = m_pApiController->GetHeight();
    int             nWidth  =   QImage::Format_Indexed8 == OutImage.format()
                                ? m_pApiController->GetWidth()
                                : m_pApiController->GetWidth() * NUM_COLORS;

    if ( OutImage.bytesPerLine() != nWidth )
    {
        for ( int y=0; y<nHeight; ++y )
        {
            pCursor = OutImage.scanLine( y );
            for ( int x=0; x<nWidth; ++x )
            {
                *pCursor = *pInBuffer;
                ++pCursor;
                ++pInBuffer;
            }
        }
    }
    else
    {
        memcpy( OutImage.bits(), pInBuffer, nWidth * nHeight );
    }
}

//
// Queries and lists all known camera
//
void SynchronousGrab::UpdateCameraListBox()
{
    // Get all cameras currently connected to Vimba
    CameraPtrVector cameras = m_pApiController->GetCameraList();

    // Simply forget about all cameras known so far
    ui.m_ListBoxCameras->clear();
    m_cameras.clear();

    // And query the camera details again
    for (   CameraPtrVector::const_iterator iter = cameras.begin();
            cameras.end() != iter;
            ++iter )
    {
        std::string         strCameraName;
        std::string         strCameraID;
        if ( VmbErrorSuccess != (*iter)->GetName( strCameraName ))
        {
            strCameraName = "[NoName]";
        }
        // If for any reason we cannot get the ID of a camera we skip it
        if ( VmbErrorSuccess == (*iter)->GetID( strCameraID ))
        {
            ui.m_ListBoxCameras->addItem( QString::fromStdString( strCameraName + " " +strCameraID ) );
            m_cameras.push_back( strCameraID );
        }
    }

    ui.m_ButtonStartStop->setEnabled( 0 < m_cameras.size() || m_bIsStreaming );
}

//
// Prints out a given logging string, error code and the descriptive representation of that error code
//
// Parameters:
//  [in]    strMsg          A given message to be printed out
//  [in]    eErr            The API status code
//
void SynchronousGrab::Log( std::string strMsg, VmbErrorType eErr )
{
    strMsg +=  "..." + m_pApiController->ErrorCodeToMessage( eErr );
    ui.m_ListLog->insertItem( 0, QString::fromStdString( strMsg ) );
}

//
// Prints out a given logging string
//
// Parameters:
//  [in]    strMsg          A given message to be printed out
//
void SynchronousGrab::Log( std::string strMsg )
{
    ui.m_ListLog->insertItem( 0, QString::fromStdString( strMsg ) );
}

//
//Takes a single image
//
void SynchronousGrab::SingleImageTaskSynchro(int nRow){
    VmbErrorType    err;
    // Get the frame
    FramePtr pFrame;
    err = m_pApiController->AcquireSingleImage( m_cameras[nRow], pFrame );
    if ( VmbErrorSuccess == err )
    {
        // See if it is not corrupt
        VmbFrameStatusType eReceiveStatus;
        err = pFrame->GetReceiveStatus( eReceiveStatus );
        if (    VmbErrorSuccess == err
             && VmbFrameStatusComplete == eReceiveStatus )
        {
            // Set up Qt image
            QImage  tmpImage(   m_pApiController->GetWidth(),
                                m_pApiController->GetHeight(),
                                VmbPixelFormatRgb8 == m_pApiController->GetPixelFormat() ? QImage::Format_RGB888 : QImage::Format_Indexed8 );

            if(VmbPixelFormatRgb8 != m_pApiController->GetPixelFormat())
            {
                tmpImage.setNumColors(256);
                for(int i = 0; i < 256; i++)
                {
                    tmpImage.setColor(i, qRgb(i, i, i));
                }
            }

            VmbUchar_t *pBuffer;
            err = pFrame->GetImage( pBuffer );
            if ( VmbErrorSuccess == err )
            {
                VmbUint32_t nSize;
                err = pFrame->GetImageSize( nSize );
                if ( VmbErrorSuccess == err )
                {
                    VmbPixelFormatType ePixelFormat = m_pApiController->GetPixelFormat();
                    if (    VmbPixelFormatMono8 == ePixelFormat
                         || VmbPixelFormatRgb8 == ePixelFormat )
                    {
                        // Copy it
                        // We need that because Qt might repaint the view after we have released the frame already
                        CopyToImage( pBuffer, tmpImage );

                        // Display it
                        QSize s= ui.m_LabelStream->size();
                        ui.m_LabelStream->setPixmap( QPixmap::fromImage( tmpImage ).scaled( s, Qt::KeepAspectRatio) );
                    }
                }
            }

            Log( "Starting Acquisition", err );
            m_bIsStreaming = VmbErrorSuccess == err;
        }
        else
        {
            // If we receive an incomplete image we do nothing
            err = VmbErrorOther;
        }
    }
    Log( "Acquire single image", err );
}

