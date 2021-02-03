#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "changefeatures.h"

#include "VimbaCPP/Include/VimbaCPP.h"
#include "VimbaCPP/Examples/Common/StreamSystemInfo.h"
#include "VimbaCPP/Examples/Common/ErrorCodeToMessage.h"

namespace AVT {
namespace VmbAPI {
namespace Examples {

//
// Prints out all details of a feature
//
// Parameters:
//  [in]    feature         The feature to work on
//
std::string PrintFeatures( CameraPtr &camera, std::string name)
{
    std::stringstream ss;
    FeaturePtr feature;
    double value;

    const char* cname = name.c_str();//transform std::string into const char*, because function "GetFeatureByName" needs it

    if (VmbErrorSuccess == camera->GetFeatureByName(cname, feature)) //We obtain the parameter we want and then assign it to "feature" pointer
    {
        if (VmbErrorSuccess == feature->GetValue(value))
        {
            ss << "\nCurrent value for " << name << ": " << value;//once we get the vale, we print it
        }
    }
    else
    {
        ss << "Could not obtain the value for " << name;
    }
    std::string sstext = ss.str();
    return sstext;
}

std::string WriteFeature(CameraPtr &camera, std::string name, double value)
{
    std::stringstream ss;
    FeaturePtr feature;
    const char* cname = name.c_str();//transformo std::string en const char*, puesto que lo requiere la funcion GetFeatureByName

    if(VmbErrorSuccess == camera->GetFeatureByName(cname, feature))
    {
        if(VmbErrorSuccess == feature->SetValue(value))
        {
            ss << "\nValue for " << name << " successfully changed.";
        }
        else
        {
            ss << "\nFeature could not be changed.";
        }
    }
    std::string sstext = ss.str();
    return sstext;
}
std::string ChangeFeatures::Print(std::string CameraID, std::string name, double value)
{
    std::stringstream textstream;
    VimbaSystem&        sys         = VimbaSystem::GetInstance();           // Get a reference to the VimbaSystem singleton
    textstream << "Vimba C++ API Version " << sys << "\n";                          // Print out version of Vimba
    VmbErrorType        err         = sys.Startup();                        // Initialize the Vimba API
    //FeaturePtrVector    features;                                           // A vector of std::shared_ptr<AVT::VmbAPI::Feature> objects

    CameraPtr           pCamera     = CameraPtr();                          // Our camera

    if( VmbErrorSuccess == err )
    {
        if( CameraID.empty() )                                              // If no ID was provided use the first camera
        {
            CameraPtrVector cameras;
            err = sys.GetCameras( cameras );
            if(     VmbErrorSuccess == err
                &&  !cameras.empty() )
            {
                err = cameras[0]->Open( VmbAccessModeFull );                // Open the camera
                if( VmbErrorSuccess == err )
                {
                    pCamera = cameras[0];
                    err = pCamera->GetID( CameraID );
                }
            }
        }
        else
        {
            err = sys.OpenCameraByID( CameraID.c_str(), VmbAccessModeFull, pCamera ); // Get and open the camera
        }
        if( NULL != pCamera )
        {

            //Changing parameter value
            textstream << PrintFeatures(pCamera, name);
            textstream << "\nChanging value for " << name;
            textstream << WriteFeature(pCamera, name, value);
            textstream << PrintFeatures(pCamera, name);

            pCamera->Close();
        }
        else
        {
            textstream << "Could not open camera or no camera available. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "\n";
        }
        sys.Shutdown();
    }
    else
    {
        textstream << "Could not start system. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "\n";
    }
    std::string textoventana = textstream.str();
    return textoventana;
}

}}} // namespace AVT::VmbAPI::Examples

