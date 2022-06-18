#include "InnerOption.hpp"

ws::InnerOption::InnerOption() {
  this->_root = "~/webserv/www/";
  this->_index.push_back("index.html");
  this->_client_max_body_size = 1024 * 1024;
  this->_directory_flag = false;
}

ws::InnerOption::~InnerOption() {}
