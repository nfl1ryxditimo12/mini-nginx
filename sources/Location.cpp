#include "Location.hpp"

ws::Location::Location()
{
    this->_return = std::pair<int, std::string>(); // 나중에 찍어봐야함
    this->_cgi = std::pair<std::string, std::string>();
}

ws::Location::~Location() {}
