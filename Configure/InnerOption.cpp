#include "InnerOption.hpp"

#include <stdexcept>

ws::InnerOption::InnerOption() : _client_max_body_size(ULONG_MAX), _directory_list(-1) {
  // this->_root = "~/webserv/www/";
  // this->_index.push_back("index.html");
  // this->_client_max_body_size = 1024 * 1024;
  // this->_directory_flag = false;
}

ws::InnerOption::~InnerOption() {}

void ws::InnerOption::set_client_max_body_size(unsigned long client_max_body_size) throw() {
  _client_max_body_size = client_max_body_size;
}

void ws::InnerOption::set_directory_list(const std::string& value) {
  if (value == "on")
    _directory_list = 1;
  else if (value == "off")
    _directory_list = 0;
  else
    throw std::invalid_argument("Configure: wrong directory list value");
}

void ws::InnerOption::set_root(const std::string& value) {
  
}
