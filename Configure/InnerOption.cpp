#include "InnerOption.hpp"

#include <stdexcept>

ws::InnerOption::InnerOption() : _autoindex(kAUTOINDEX_UNSET), _client_max_body_size(kCLIENT_MAX_BODY_SIZE_UNSET) {
  // this->_root = "~/webserv/www/";
  // this->_index.push_back("index.html");
  // this->_client_max_body_size = 1024 * 1024;
  // this->_directory_flag = false;
}

ws::InnerOption::~InnerOption() {}

const ws::InnerOption::autoindex_type& ws::InnerOption::get_autoindex() const throw() {
  return _autoindex;
}

const ws::InnerOption::root_type& ws::InnerOption::get_root() const throw() {
  return _root;
}

const ws::InnerOption::index_type& ws::InnerOption::get_index() const throw() {
  return _index;
}

const ws::InnerOption::client_max_body_size_type& ws::InnerOption::get_client_max_body_size() const throw() {
  return _client_max_body_size;
}

const ws::InnerOption::error_page_type& ws::InnerOption::get_error_page() const throw() {
  return _error_page;
}

void ws::InnerOption::set_autoindex(const autoindex_type& value) {
  _autoindex = value;
}

void ws::InnerOption::set_root(const root_type& value) {
  _root = value;
}

void ws::InnerOption::set_index(const index_value_type& value) {
  _index.push_back(value);
}

void ws::InnerOption::set_client_max_body_size(const client_max_body_size_type& value) {
  _client_max_body_size = value;
}

void ws::InnerOption::set_error_page(const error_page_value_type& value) {
  _error_page.insert(value);
}

