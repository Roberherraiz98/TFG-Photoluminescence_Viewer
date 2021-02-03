#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <listfeatures.h>

#include "VimbaCPP/Include/VimbaCPP.h"
#include "VimbaCPP/Examples/Common/StreamSystemInfo.h"
#include "VimbaCPP/Examples/Common/ErrorCodeToMessage.h"

namespace AVT {
namespace VmbAPI {
namespace Examples {

//
// Prints out an error message in case a feature's value could not be queried.
// Prints nothing in case the error means success.
//
// Parameters:
//  [in]    err             The return code indicating the error
//
// Returns:
//  The error code as passed in
//
VmbErrorType PrintGetValueErrorMessage ( const VmbErrorType err )
{
    if ( VmbErrorSuccess != err )
    {
        std::cout << "Could not get feature value. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "\n";
    }

    return err;
}

//
// Prints out the value and the type of a given feature
//
// Parameters:
//  [in]    feature         The feature to work on
//
std::string PrintFeatureValue( const FeaturePtr &feature )
{
    std::stringstream ss;
    VmbFeatureDataType  eType;
    VmbErrorType        err     = feature->GetDataType( eType );
    if( VmbErrorSuccess != err )
    {
        ss << "[Could not get feature Data Type. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]\n";
    }
    else
    {
        ss << "/// Value          : ";
        switch( eType )
        {
            case VmbFeatureDataBool:
                {
                    VmbBool_t bValue;
                    err = feature->GetValue( bValue );
                    if ( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                    {
                        ss << bValue << "\n";
                    }
                    ss << "/// Type           : Boolean\n";
                }
                break;
            case VmbFeatureDataEnum:
                {
                    std::string strValue;
                    err = feature->GetValue( strValue );
                    if ( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                    {
                        ss << strValue << "\n";
                    }
                    ss << "/// Type           : Enumeration\n";
                }
                break;
            case VmbFeatureDataFloat:
                {
                    double fValue;
                    double fMin, fMax;
                    err = feature->GetValue( fValue );
                    if( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                    {
                        ss << fValue << "\n";
                    }

                    ss << "/// Minimum        : ";
                    err = feature->GetRange( fMin, fMax );
                    if( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                    {
                        ss << fMin << "\n";
                        ss << "/// Maximum        : " << fMax << "\n";
                    }
                    ss << "/// Type           : Double precision floating point\n";
                }
                break;
            case VmbFeatureDataInt:
                {
                    VmbInt64_t nValue;
                    VmbInt64_t nMin, nMax;
                    err = feature->GetValue( nValue );
                    if( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                    {
                        ss << nValue << "\n";
                    }

                    ss << "/// Minimum        : ";
                    err = feature->GetRange( nMin, nMax );
                    if( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                    {
                        ss << nMin << "\n";
                        ss << "/// Maximum        : " << nMax << "\n";
                    }
                    ss << "/// Type           : Long long integer\n";
                }
                break;
            case VmbFeatureDataString:
                {
                    std::string strValue;

                    err = feature->GetValue( strValue );
                    if( VmbErrorSuccess == PrintGetValueErrorMessage( err ) )
                    {
                        ss << strValue << "\n";
                    }
                    ss << "/// Type           : String\n";
                }
                break;
            case VmbFeatureDataCommand:
            default:
                {
                    ss << "[None]" << "\n";
                    ss << "/// Type           : Command feature\n";
                }
                break;
        }

        ss << "\n";
    }
    std::string sstext = ss.str();
    return sstext;
}

//
// Prints out all details of a feature
//
// Parameters:
//  [in]    feature         The feature to work on
//
std::string PrintFeatures( const FeaturePtr &feature )
{
    std::stringstream ss;
    std::string strName;                                                    // The name of the feature
    std::string strDisplayName;                                             // The display name of the feature
    std::string strToolTip;                                                 // A short description of the feature
    std::string strDescription;                                             // A long description of the feature
    std::string strCategory;                                                // A category to group features
    std::string strSFNCNamespace;                                           // The Standard Feature Naming Convention namespace
    std::string strUnit;                                                    // The measurement unit of the value

    std::ostringstream ErrorStream;

    VmbErrorType err = feature->GetName( strName );
    if( VmbErrorSuccess != err )
    {
        ErrorStream << "[Could not get feature Name. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
        strName = ErrorStream.str();
    }

    err = feature->GetDisplayName( strDisplayName );
    if( VmbErrorSuccess != err )
    {
        ErrorStream << "[Could not get feature Display Name. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
        strDisplayName = ErrorStream.str();
    }

    err = feature->GetToolTip( strToolTip );
    if( VmbErrorSuccess != err )
    {
        ErrorStream << "[Could not get feature Tooltip. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
        strToolTip = ErrorStream.str();
    }

    err = feature->GetDescription( strDescription );
    if( VmbErrorSuccess != err )
    {
        ErrorStream << "[Could not get feature Description. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
        strDescription = ErrorStream.str();
    }

    err = feature->GetCategory( strCategory );
    if( VmbErrorSuccess != err )
    {
        ErrorStream << "[Could not get feature Category. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
        strCategory = ErrorStream.str();
    }

    err = feature->GetSFNCNamespace( strSFNCNamespace );
    if( VmbErrorSuccess != err )
    {
        ErrorStream << "[Could not get feature SNFC Namespace. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
        strSFNCNamespace = ErrorStream.str();
    }

    err = feature->GetUnit( strUnit );
    if( VmbErrorSuccess != err )
    {
        ErrorStream << "[Could not get feature Unit. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "]";
        strUnit = ErrorStream.str();
    }

    ss << "/// Feature Name   : " << strName             << "\n";
    ss << "/// Display Name   : " << strDisplayName      << "\n";
    ss << "/// Tooltip        : " << strToolTip          << "\n";
    ss << "/// Description    : " << strDescription      << "\n";
    ss << "/// SNFC Namespace : " << strSFNCNamespace    << "\n";
    ss << "/// Unit           : " << strUnit             << "\n";

    ss << PrintFeatureValue( feature );

    std::string sstext = ss.str();
    return sstext;
}

//
// Prints out all features and their values and details of a given camera.
// If no camera ID is provided, the first camera will be used.
// Starts and stops the API
// Opens and closes the camera
//
// Parameters:
//  [in]    CameraID        The ID of the camera to work
//
std::string ListFeatures::Print( std::string CameraID )
{
    std::stringstream textstream;//guardare aqui todo lo que imprima para luego convertirlo en una variable std::string
    VimbaSystem&        sys         = VimbaSystem::GetInstance();           // Get a reference to the VimbaSystem singleton
    textstream << "Vimba C++ API Version " << sys << "\n";                          // Print out version of Vimba
    VmbErrorType        err         = sys.Startup();                        // Initialize the Vimba API
    FeaturePtrVector    features;                                           // A vector of std::shared_ptr<AVT::VmbAPI::Feature> objects
    CameraPtr           pCamera     = CameraPtr();                          // Our camera

    std::stringstream strError;

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
            textstream << "Printing all features of camera with ID: " << CameraID << "\n";
            err = pCamera->GetFeatures( features );                         // Fetch all features of our cam
            if( VmbErrorSuccess == err )
            {
                // Query all static details as well as the value of all fetched features and print them out.
                std::for_each( features.begin(), features.end(), PrintFeatures );
                for(unsigned i = 0; i < features.size(); i++){ //recorre todo el vector de parametros disponibles mostrando su informacion
                    textstream << PrintFeatures(features[i]);
                }
            }
            else
            {
                textstream << "Could not get features. Error code: " << err << " (" << AVT::VmbAPI::Examples::ErrorCodeToMessage( err ) << ")" << "\n";
            }

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
