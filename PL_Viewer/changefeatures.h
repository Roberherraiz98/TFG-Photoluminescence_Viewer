#ifndef CHANGEFEATURES_H
#define CHANGEFEATURES_H


#include <string>

namespace AVT {
namespace VmbAPI {
namespace Examples {

class ChangeFeatures
{
public:

    static std::string Print( std::string CameraID, std::string name, double value );
};

}}} // namespace AVT::VmbAPI::Examples

#endif // CHANGEFEATURES_H
