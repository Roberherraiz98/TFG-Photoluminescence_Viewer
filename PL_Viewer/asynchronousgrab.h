#ifndef ASYNCHRONOUSGRAB_H
#define ASYNCHRONOUSGRAB_H

#include <QDialog>
#include "ui_asynchronousgrab.h"
#include <VimbaCPP/Examples/AsynchronousGrab/Qt/Source/AApiController.h>
#include <VimbaCPP/Examples/SynchronousGrab/Qt/Source/ApiController.h>
#include "retardtime.h"
#include "pwmduration.h"

#include <wiringPi.h>
#include <softPwm.h>

//#include <mutex>

using AVT::VmbAPI::Examples::AApiController;

class AsynchronousGrab : public QDialog
{
    Q_OBJECT

public:
    AsynchronousGrab( QWidget *parent = 0, Qt::WindowFlags flags = 0 );
    ~AsynchronousGrab();
    std::string SetRetardTime(int);
    std::string SetPWMDuration(int);
    int GetRetardTime();//this function will grant access to retardtime value from external classes
    int GetPWMDuration();//this function will grant access to pwmdurationtime value from external classes
    void SingleImageTask(int);



private:
    // The Qt GUI
    Ui::AsynchronousGrab ui;



    // Our controller that wraps API access
    AApiController              m_ApiController;

    // A list of known camera IDs
    std::vector<std::string> m_cameras;
    // Are we streaming?
    bool m_bIsStreaming;
    // Our Qt image to display
    QImage m_Image;
    QImage p_Image;
    //std::mutex mtx; //Mutex to avoid certain threads simultaneously execution



    //
    // Queries and lists all known camera
    //
    void UpdateCameraListBox();

    //
    // Prints out a given logging string, error code and the descriptive representation of that error code
    //
    // Parameters:
    //  [in]    strMsg          A given message to be printed out
    //  [in]    eErr            The API status code
    //
    void Log( std::string strMsg, VmbErrorType eErr );

    //
    // Prints out a given logging string
    //
    // Parameters:
    //  [in]    strMsg          A given message to be printed out
    //
    void Log( std::string strMsg);

    //
    // Copies the content of a byte buffer to a Qt image with respect to the image's alignment
    //
    // Parameters:
    //  [in]    pInbuffer       The byte buffer as received from the cam
    //  [in]    ePixelFormat    The pixel format of the frame
    //  [out]   OutImage        The filled Qt image
    //
    VmbErrorType CopyToImage( VmbUchar_t *pInBuffer, VmbPixelFormat_t ePixelFormat, QImage &pOutImage, const float *Matrix = NULL );
    void CopyToImage( VmbUchar_t *pInBuffer, QImage &OutImage );

    std::string CurrentTime();

private slots:
    // The event handler for starting / stopping acquisition
    void OnBnClickedButtonStartstop();

    //
    // This event handler (Qt slot) is triggered through a Qt signal posted by the frame observer
    //
    // Parameters:
    //  [in]    status          The frame receive status (complete, incomplete, ...)
    //
    void OnFrameReady( int status );

    //
    // This event handler (Qt slot) is triggered through a Qt signal posted by the camera observer
    //
    // Parameters:
    //  [in]    reason          The reason why the callback of the observer was triggered (plug-in, plug-out, ...)
    //
    void OnCameraListChanged( int reason );
    friend class retardtime;
    friend class pwmduration;
};

#endif
