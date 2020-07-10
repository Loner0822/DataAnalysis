#include <string>

const int PackageLength = 158;

class Package {
public:
    std::string Package_Leader;
    std::string Package_Sub_Leader;
    std::string Package_Data;

    Package();
    Package(const std::string& pkg_string);
    ~Package();
};