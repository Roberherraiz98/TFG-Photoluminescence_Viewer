#ifndef SYNCHRONOUSGRAB_H
#define SYNCHRONOUSGRAB_H


#include <QDialog>
#include "ui_synchronousgrab.h"
#include <VimbaCPP/Examples/SynchronousGrab/Qt/Source/ApiController.h>

#include <wiringPi.h>
#include <softPwm.h>


using AVT::VmbAPI::Examples::ApiController;

class SynchronousGrab : public QDialog
{
    Q_OBJECT

public:
    SynchronousGrab(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~SynchronousGrab();



private:
    Ui::SynchronousGrab ui;

    // Our controller that wraps API access
        ApiController *             m_pApiController;
        // A list of known camera IDs
        std::vector<std::string>    m_cameras;
        // Are we streaming?
        bool                        m_bIsStreaming;

        //
        // Queries and lists all known camera
        //
        void UpdateCameraListBox();
        void SingleImageTaskSynchro(int);

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
        void Log( std::string strMsg );

        //
        // Copies the content of a byte buffer to a Qt image with respect to the image's alignment
        //
        // Parameters:
        //  [in]    pInbuffer       The byte buffer as received from the cam
        //  [out]   OutImage        The filled Qt image
        //
        void CopyToImage( VmbUchar_t *pInBuffer, QImage &OutImage );

    private slots:
        void OnBnClickedButtonStartstop();

        //
        // This event handler (Qt slot) is triggered through a Qt signal posted by the camera observer
        //
        // Parameters:
        //  [in]    reason          The reason why the callback of the observer was triggered (plug-in, plug-out, ...)
        //
        void OnCameraListChanged( int reason );
};

#endif // SYNCHRONOUSGRAB_H
