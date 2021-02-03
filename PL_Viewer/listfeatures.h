#ifndef AVT_VMBAPI_EXAMPLES_LISTFEATURES
#define AVT_VMBAPI_EXAMPLES_LISTFEATURES

#include <string>

namespace AVT {
namespace VmbAPI {
namespace Examples {

class ListFeatures
{
public:
    //
    // Prints out all features and their values and details of a given camera.
    // If no camera ID is provided, the first camera will be used.
    // Starts and stops the API
    // Opens and closes the camera
    //
    // Parameters:
    //  [in]    CameraID        The ID of the camera to work
    //
    static std::string Print( std::string CameraID );
};

}}} // namespace AVT::VmbAPI::Examples

#endif
