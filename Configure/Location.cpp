#include "Location.hpp"
#include <iostream> // todo
ws::Location::Location() : _return(return_type(0, "")), _cgi("", "") {
  init_limit_except();
  // this->_return = std::pair<int, std::string>();  // 나중에 찍어봐야함
  // this->_cgi = std::pair<std::string, std::string>();
}

ws::Location::~Location() {}

void ws::Location::init_limit_except() {
  _limit_except.insert(limit_except_type::value_type("GET", -1));
  _limit_except.insert(limit_except_type::value_type("POST", -1));
  _limit_except.insert(limit_except_type::value_type("DELETE", -1));
  _limit_except.insert(limit_except_type::value_type("HEAD", -1));
}

ws::Location::limit_except_type ws::Location::get_limit_except() const throw() {
  return _limit_except;
}

ws::Location::return_type ws::Location::get_return_type() const throw() {
  return _return;
}

ws::Location::cgi_type ws::Location::get_cgi() const throw() {
  return _cgi;
}

void ws::Location::set_limit_except(const std::string& method, bool value) {
  _limit_except.find(method)->second = value;
  std::cout << method << ", " << value << std::endl;
}

void ws::Location::set_return_type(const return_type& value) {
  _return = value;
  std::cout << value.first << ", " << value.second << std::endl;
}

void ws::Location::set_option(const ws::InnerOption& value) {
  _option = value;
}

void ws::Location::set_cgi(const cgi_type& value) {
  _cgi = value;
  std::cout << value << std::endl;
}
