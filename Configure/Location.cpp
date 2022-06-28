#include "Location.hpp"

ws::Location::Location() : _return(return_type(0, "")), _cgi("", "") {
  init_limit_except();
  // this->_return = std::pair<int, std::string>();  // 나중에 찍어봐야함
  // this->_cgi = std::pair<std::string, std::string>();
}

ws::Location::~Location() {}

void ws::Location::init_limit_except() {
  _limit_except_map.insert(limit_except_map_type::value_type("GET", -1));
  _limit_except_map.insert(limit_except_map_type::value_type("POST", -1));
  _limit_except_map.insert(limit_except_map_type::value_type("DELETE", -1));
  _limit_except_map.insert(limit_except_map_type::value_type("HEAD", -1));
}

const ws::Location::limit_except_map_type& ws::Location::get_limit_except() const throw() {
  return _limit_except_map;
}

const ws::Location::return_type& ws::Location::get_return_type() const throw() {
  return _return;
}

const ws::Location::cgi_type& ws::Location::get_cgi() const throw() {
  return _cgi;
}

const ws::InnerOption& ws::Location::get_option() const throw() {
  return _option;
}

void ws::Location::set_limit_except(const std::string& method, bool value) {
  _limit_except_map.find(method)->second = value;
}

void ws::Location::set_return_type(const return_type& value) {
  _return = value;
}

void ws::Location::set_option(const ws::InnerOption& value) {
  _option = value;
}

void ws::Location::set_cgi(const cgi_type& value) {
  _cgi = value;
}
