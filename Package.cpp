#include <iostream>
#include "Package.h"

Package::Package() {
    Package::Package_Leader = "";
    Package::Package_Sub_Leader = "";
    Package::Package_Data = "";
}

Package::Package(const std::string& pkg_string) {
    if (pkg_string.size() != PackageLength) {
        std::cout << "Illegal (Package)String" << std::endl;
        return;
    }
    Package::Package_Leader = pkg_string.substr(0, 12);
    Package::Package_Sub_Leader = pkg_string.substr(12, 10);
    Package::Package_Data = pkg_string.substr(22, 136);
}

Package::~Package() {

}