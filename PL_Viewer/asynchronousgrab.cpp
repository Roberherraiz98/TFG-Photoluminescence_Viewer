#include <sstream>
#include "asynchronousgrab.h"
#include "ui_asynchronousgrab.h"
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

#include "VimbaImageTransform/Include/VmbTransform.h"
#include "VimbaImageTransform/Include/VmbTransformTypes.h"
#include "VimbaImageTransform/Include/VmbCommonTypes.h"
#define NUM_COLORS 3
#define BIT_DEPTH 8

#define GPIO_PIN 6

#define CAMERA_RETARD 2560

using AVT::VmbAPI::FramePtr;
using AVT::VmbAPI::CameraPtrVector;

int retardtime = 0;
int pwmdurationtime = 0;


void FlashTask(){

    if(retardtime >= 0){
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


    std::string str(buffer);
    std::cout<< "Flash: " <<str << "." << milisec << std::endl;

    delay(pwmdurationtime);
    softPwmWrite(0, 0);

}

void RetardTask(){//Applies the retardtime
    if(retardtime<0){
        delay(abs(retardtime) + CAMERA_RETARD);
    }
    else
        delay(abs(retardtime));

}
void RetardFlashTask(){//Combines Retard + Flash and it is used when retardtime is < 0
    RetardTask();
    FlashTask();
}

void AsynchronousGrab::SingleImageTask(int nRow){

    // Get the frame
    FramePtr pFrame;

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

    std::string str(buffer);
    std::cout<< "Acquisition: " <<str << "." << milisec << std::endl;

    VmbErrorType err = m_ApiController.AcquireSingleImage( m_cameras[nRow], pFrame );


    if ( VmbErrorSuccess == err )
    {
        // See if it is not corrupt
        VmbFrameStatusType eReceiveStatus;
        err = pFrame->GetReceiveStatus( eReceiveStatus );
        if (    VmbErrorSuccess == err
             && VmbFrameStatusComplete == eReceiveStatus )
        {
            // Set up Qt image
           p_Image =  QImage  (   m_ApiController.GetWidth(),
                                m_ApiController.GetHeight(),
                                VmbPixelFormatRgb8 == m_ApiController.GetPixelFormat() ? QImage::Format_RGB888 : QImage::Format_Indexed8 );

            if(VmbPixelFormatRgb8 != m_ApiController.GetPixelFormat())
            {
                p_Image.setNumColors(256);
                for(int i = 0; i < 256; i++)
                {
                    p_Image.setColor(i, qRgb(i, i, i));
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
                    VmbPixelFormatType ePixelFormat = m_ApiController.GetPixelFormat();
                    if (    VmbPixelFormatMono8 == ePixelFormat
                         || VmbPixelFormatRgb8 == ePixelFormat )
                    {
                        // Copy it
                        // We need that because Qt might repaint the view after we have released the frame already
                        CopyToImage( pBuffer, p_Image );

                        // Display it
                        QSize s= ui.m_LabelStream->size();
                        ui.m_LabelStream->setPixmap( QPixmap::fromImage( p_Image ).scaled( s, Qt::KeepAspectRatio) );
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
    m_bIsStreaming = false;

}


std::string AsynchronousGrab::SetRetardTime(int t){
    std::stringstream ss;
    ss<<"Previous value for retard time: "<<retardtime<<std::endl;
    retardtime = t;
    ss<<"Value succesfully modified, new value: "<<retardtime<<std::endl;
    std::string sstext = ss.str();
    return sstext;
}

std::string AsynchronousGrab::SetPWMDuration(int t){
    std::stringstream ss;
    ss<<"Previous value for PWM signal duration: "<<pwmdurationtime<<std::endl;
    pwmdurationtime = t;
    ss<<"Value succesfully changed, new value: "<<pwmdurationtime<<std::endl;
    std::string sstext = ss.str();
    return sstext;
}

int AsynchronousGrab::GetRetardTime(){
    return retardtime;
}

int AsynchronousGrab::GetPWMDuration(){
    return pwmdurationtime;
}

AsynchronousGrab::AsynchronousGrab( QWidget *parent, Qt::WindowFlags flags )
    : QDialog( parent, flags )
    , m_bIsStreaming( false )
{
    ui.setupUi( this );
    ui.m_LabelStream->setAlignment(Qt::AlignCenter );
    // Connect GUI events with event handlers
    QObject::connect( ui.m_ButtonStartStop, SIGNAL( clicked() ), this, SLOT( OnBnClickedButtonStartstop() ) );

    // Start Vimba
    VmbErrorType err = m_ApiController.StartUp();
    setWindowTitle( QString( "Asynchronous Grab" ) );
    Log( "Starting Vimba", err );

    if( VmbErrorSuccess == err )
    {
        // Connect new camera found event with event handler
        QObject::connect( m_ApiController.GetCameraObserver(), SIGNAL( CameraListChangedSignal(int) ), this, SLOT( OnCameraListChanged(int) ) );

        // Initially get all connected cameras
        UpdateCameraListBox();
        std::stringstream strMsg;
        strMsg << "Cameras found..." << m_cameras.size();
        Log(strMsg.str() );
    }
}

AsynchronousGrab::~AsynchronousGrab()
{
    // if we are streaming stop streaming
    if( true == m_bIsStreaming )
    {
        OnBnClickedButtonStartstop();
    }

    // Before we close the application we stop Vimba
    m_ApiController.ShutDown();
}

void AsynchronousGrab::OnBnClickedButtonStartstop()
{
    VmbErrorType err;
    int nRow = ui.m_ListBoxCameras->currentRow();
    std::stringstream ss;

    //SHOWING CURRENT PARAMETERS WHILE ACQUIRING IMAGES
    //creating a "sss" stringstream in order to show it on the interface
    std::stringstream sss;
    sss << "Current retard time: " << retardtime << " ms \n";
    sss << "Current flash duration: " << pwmdurationtime << " ms \n";
    std::string sstext = sss.str();
    ui.CurrentParameters->setText(QString::fromStdString(sstext));


    if( -1 < nRow )
    {
        if( false == m_bIsStreaming )
        {
            // Start acquisition
            err = m_ApiController.StartContinuousImageAcquisition( m_cameras[nRow] );
            // Set up Qt image
            if ( VmbErrorSuccess == err )
            {
                m_Image = QImage( m_ApiController.GetWidth(),
                                  m_ApiController.GetHeight(),
                                  QImage::Format_RGB888 );

                QObject::connect( m_ApiController.GetFrameObserver(), SIGNAL( FrameReceivedSignal(int) ), this, SLOT( OnFrameReady(int) ) );
            }
            Log( "Starting Acquisition", err );
            m_bIsStreaming = VmbErrorSuccess == err;
        }
        else //when "stop acquisition" button is clicked
        {
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

            std::string str(buffer);
            std::cout<< "Stop: " <<str << "." << milisec << std::endl;

            m_bIsStreaming = false;
            // Stop acquisition
            err = m_ApiController.StopContinuousImageAcquisition();

            // Clear all frames that we have not picked up so far
            m_ApiController.ClearFrameQueue();
            Log( "Stopping Acquisition", err );


            if (retardtime >= 0){

                std::thread t1(FlashTask);
                std::thread t2(RetardTask);


                t1.detach();//do not wait for t1 to end
                t2.join();//wait for t2 to end until continuing compilation

                AsynchronousGrab::SingleImageTask(nRow);
            }

            else//retard time < 0
            {

                std::thread t1(RetardFlashTask);
                std::thread t2(&AsynchronousGrab::SingleImageTask, this, nRow);

                t2.join();
                t1.detach();
            }

            //SAVING THE LAST FRAME TO A FILE

            const QSize s = ui.m_LabelStream->size() ;
            ui.m_LabelStream->setPixmap( QPixmap::fromImage( p_Image ).scaled(s,Qt::KeepAspectRatio ) );//set the pixmap for the last frame

            ss << "/home/ubuntu/Vimba_Images/" << CurrentTime() << ".png";//create the filename with the current date & time
            std::string ImagePath = ss.str();
            const char* cImagePath = ImagePath.c_str();//Convert std::string into char*


            QPixmap::fromImage(p_Image).save(cImagePath);

            //TIME STAMP
            time_t _rawtime;
            int _milisec;
            struct tm * _timeinfo;
            struct timeval _tv;
            char _buffer[80];

            gettimeofday(&_tv, NULL);

            _milisec = lrint(_tv.tv_usec/1000.0);
            if (_milisec>=1000){
                _milisec -=1000;
                _tv.tv_sec++;
            }

            time(&_rawtime);
            _timeinfo = localtime(&_rawtime);

            strftime(_buffer, sizeof(_buffer), "%d_%m_%Y_%I:%M:%S", _timeinfo);

            std::string _str(_buffer);
            std::cout<< "Saved: " <<_str << "." << _milisec << std::endl;


            Log("Last frame saved to file");
            p_Image = QImage();//clearing image after saving
        }

        if( false == m_bIsStreaming )
        {
            ui.m_ButtonStartStop->setText( QString( "Start Image Acquisition" ) );
        }
        else
        {
            ui.m_ButtonStartStop->setText( QString( "Stop Image Acquisition" ) );
        }
    }
}

//
// This event handler (Qt slot) is triggered through a Qt signal posted by the frame observer
//
// Parameters:
//  [in]    status          The frame receive status (complete, incomplete, ...)
//
void AsynchronousGrab::OnFrameReady( int status )
{
    if( true == m_bIsStreaming )
    {
        // Pick up frame
        FramePtr pFrame = m_ApiController.GetFrame();
        if( SP_ISNULL( pFrame ) )
        {
            Log("frame pointer is NULL, late frame ready message");
            return;
        }
        // See if it is not corrupt
        if( VmbFrameStatusComplete == status )
        {
            VmbUchar_t *pBuffer;
            VmbErrorType err = SP_ACCESS( pFrame )->GetImage( pBuffer );
            if( VmbErrorSuccess == err )
            {
                VmbUint32_t nSize;
                err = SP_ACCESS( pFrame )->GetImageSize( nSize );
                if( VmbErrorSuccess == err )
                {
                    VmbPixelFormatType ePixelFormat = m_ApiController.GetPixelFormat();
                    if( ! m_Image.isNull() )
                    {
                        // Copy it
                        // We need that because Qt might repaint the view after we have released the frame already
                        if( ui.m_ColorProcessingCheckBox->checkState()==  Qt::Checked )
                        {
                            static const VmbFloat_t Matrix[] = {    8.0f, 0.1f, 0.1f, // this matrix just makes a quick color to mono conversion
                                                                    0.1f, 0.8f, 0.1f,
                                                                    0.0f, 0.0f, 1.0f };
                            if( VmbErrorSuccess != CopyToImage( pBuffer,ePixelFormat, m_Image, Matrix ) )
                            {
                                ui.m_ColorProcessingCheckBox->setChecked( false );
                            }
                        }
                        else
                        {
                            CopyToImage( pBuffer,ePixelFormat, m_Image );
                        }

                        // Display it
                        const QSize s = ui.m_LabelStream->size() ;
                        ui.m_LabelStream->setPixmap( QPixmap::fromImage( m_Image ).scaled(s,Qt::KeepAspectRatio ) );
                    }
                }
            }
        }
        else
        {
            // If we receive an incomplete image we do nothing but logging
            Log( "Failure in receiving image", VmbErrorOther );
        }

        // And queue it to continue streaming
        m_ApiController.QueueFrame( pFrame );
    }
}



//
// This event handler (Qt slot) is triggered through a Qt signal posted by the camera observer
//
// Parameters:
//  [in]    reason          The reason why the callback of the observer was triggered (plug-in, plug-out, ...)
//
void AsynchronousGrab::OnCameraListChanged( int reason )
{
    bool bUpdateList = false;

    // We only react on new cameras being found and known cameras being unplugged
    if( AVT::VmbAPI::UpdateTriggerPluggedIn == reason )
    {
        Log( "Camera list changed. A new camera was discovered by Vimba." );
        bUpdateList = true;
    }
    else if( AVT::VmbAPI::UpdateTriggerPluggedOut == reason )
    {
        Log( "Camera list changed. A camera was disconnected from Vimba." );
        if( true == m_bIsStreaming )
        {
            OnBnClickedButtonStartstop();
        }
        bUpdateList = true;
    }

    if( true == bUpdateList )
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
//  [in]    ePixelFormat    The pixel format of the frame
//  [out]   OutImage        The filled Qt image
//
VmbErrorType AsynchronousGrab::CopyToImage( VmbUchar_t *pInBuffer, VmbPixelFormat_t ePixelFormat, QImage &pOutImage, const float *Matrix  )
{
    const int           nHeight = m_ApiController.GetHeight();
    const int           nWidth  = m_ApiController.GetWidth();

    VmbImage            SourceImage,DestImage;
    VmbError_t          Result;
    SourceImage.Size    = sizeof( SourceImage );
    DestImage.Size      = sizeof( DestImage );

    Result = VmbSetImageInfoFromPixelFormat( ePixelFormat, nWidth, nHeight, & SourceImage );
    if( VmbErrorSuccess != Result )
    {
        Log( "Could not set source image info", static_cast<VmbErrorType>( Result ) );
        return static_cast<VmbErrorType>( Result );
    }
    QString             OutputFormat;
    const int           bytes_per_line = pOutImage.bytesPerLine();
    switch( pOutImage.format() )
    {
    default:
        Log( "unknown output format",VmbErrorBadParameter );
        return VmbErrorBadParameter;
    case QImage::Format_RGB888:
        if( nWidth*3 != bytes_per_line )
        {
            Log( "image transform does not support stride",VmbErrorWrongType );
            return VmbErrorWrongType;
        }
        OutputFormat = "RGB24";
        break;
    }
    Result = VmbSetImageInfoFromString( OutputFormat.toStdString().c_str(), OutputFormat.length(),nWidth,nHeight, &DestImage );
    if( VmbErrorSuccess != Result )
    {
        Log( "could not set output image info",static_cast<VmbErrorType>( Result ) );
        return static_cast<VmbErrorType>( Result );
    }
    SourceImage.Data    = pInBuffer;
    DestImage.Data      = pOutImage.bits();
    // do color processing?
    if( NULL != Matrix )
    {
        VmbTransformInfo TransformParameter;
        Result = VmbSetColorCorrectionMatrix3x3( Matrix, &TransformParameter );
        if( VmbErrorSuccess == Result )
        {
            Result = VmbImageTransform( &SourceImage, &DestImage, &TransformParameter,1 );

        }
        else
        {
            Log( "could not set matrix t o transform info ", static_cast<VmbErrorType>( Result ) );
            return static_cast<VmbErrorType>( Result );
        }
    }
    else
    {
        Result = VmbImageTransform( &SourceImage, &DestImage,NULL,0 );
    }
    if( VmbErrorSuccess != Result )
    {
        Log( "could not transform image", static_cast<VmbErrorType>( Result ) );
        return static_cast<VmbErrorType>( Result );
    }
    return static_cast<VmbErrorType>( Result );
}

//
// Queries and lists all known camera
//
void AsynchronousGrab::UpdateCameraListBox()
{
    // Get all cameras currently connected to Vimba
    CameraPtrVector cameras = m_ApiController.GetCameraList();

    // Simply forget about all cameras known so far
    ui.m_ListBoxCameras->clear();
    m_cameras.clear();

    // And query the camera details again
    for(    CameraPtrVector::const_iterator iter = cameras.begin();
            cameras.end() != iter;
            ++iter )
    {
        std::string strCameraName;
        std::string strCameraID;
        if( VmbErrorSuccess != (*iter)->GetName( strCameraName ) )
        {
            strCameraName = "[NoName]";
        }
        // If for any reason we cannot get the ID of a camera we skip it
        if( VmbErrorSuccess == (*iter)->GetID( strCameraID ) )
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
void AsynchronousGrab::Log( std::string strMsg, VmbErrorType eErr )
{
    strMsg += "..." + m_ApiController.ErrorCodeToMessage( eErr );
    ui.m_ListLog->insertItem( 0, QString::fromStdString( strMsg ) );
}

//
// Prints out a given logging string
//
// Parameters:
//  [in]    strMsg          A given message to be printed out
//
void AsynchronousGrab::Log( std::string strMsg)
{
    ui.m_ListLog->insertItem( 0, QString::fromStdString( strMsg ) );
}

//
//Returns a string containing the current date and time
//
std::string AsynchronousGrab::CurrentTime()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d_%m_%Y_%I_%M_%S", timeinfo);
    std::string str(buffer);

    return str;
}

void AsynchronousGrab::CopyToImage( VmbUchar_t *pInBuffer, QImage &OutImage )
{
    VmbUchar_t *    pCursor = OutImage.bits();
    int             nHeight = m_ApiController.GetHeight();
    int             nWidth  =   QImage::Format_Indexed8 == OutImage.format()
                                ? m_ApiController.GetWidth()
                                : m_ApiController.GetWidth() * NUM_COLORS;

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


