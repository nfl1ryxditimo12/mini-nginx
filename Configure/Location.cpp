#include "Location.hpp"

ws::Location::Location() : _return(return_type(0, "")), _cgi("", "") {
  // this->_return = std::pair<int, std::string>();  // 나중에 찍어봐야함
  // this->_cgi = std::pair<std::string, std::string>();
}

ws::Location::Location(const ws::Location& other) 
  : _limit_except_vec(other._limit_except_vec),
  _return(other._return),
  _cgi(other._cgi),
  _option(other._option) {}

ws::Location::~Location() {}

const ws::Location::limit_except_vec_type& ws::Location::get_limit_except_vec() const throw() {
  return _limit_except_vec;
}

const ws::Location::return_type& ws::Location::get_return() const throw() {
  return _return;
}

const ws::Location::cgi_type& ws::Location::get_cgi() const throw() {
  return _cgi;
}

const ws::InnerOption& ws::Location::get_option() const throw() {
  return _option;
}

const ws::Location::block_name_type& ws::Location::get_block_name() const throw() {
  return _block_name;
}

const ws::Location::autoindex_type& ws::Location::get_autoindex() const throw() {
  return _option.get_autoindex();
}

const ws::Location::root_type& ws::Location::get_root() const throw() {
  return _option.get_root();
}

const ws::Location::index_set_type& ws::Location::get_index_set() const throw() {
  return _option.get_index_set();
}

const ws::Location::client_max_body_size_type& ws::Location::get_client_max_body_size() const throw() {
  return _option.get_client_max_body_size();
}

const ws::Location::error_page_map_type& ws::Location::get_error_page_map() const throw() {
  return _option.get_error_page_map();
}

void ws::Location::add_limit_except(const limit_except_type& value) {
  _limit_except_vec.push_back(value);
}

void ws::Location::set_return(const return_type& value) {
  _return = value;
}

void ws::Location::set_cgi(const cgi_type& value) {
  _cgi = value;
}

void ws::Location::set_option(const ws::InnerOption& value) {
  _option = value;
}

void ws::Location::set_block_name(const block_name_type &value) {
  _block_name = value;
}

void ws::Location::set_autoindex(const autoindex_type& value) {
  _option.set_autoindex(value);
}

void ws::Location::set_root(const root_type& value) {
  _option.set_root(value);
}

void ws::Location::add_index(const index_type& value) {
  _option.add_index(value);
}

void ws::Location::set_index(const ws::Location::index_set_type &value) {
  _option.set_index(value);
}

void ws::Location::set_client_max_body_size(const client_max_body_size_type& value) {
  _option.set_client_max_body_size(value);
}
