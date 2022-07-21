#include "Location.hpp"

ws::Location::Location() : _session(0), _return(return_type(0, "")) {}

ws::Location::Location(const ws::Location& other) 
  : _session(other._session),
  _limit_except_vec(other._limit_except_vec),
  _return(other._return),
  _cgi_map(other._cgi_map),
  _option(other._option),
  _block_name(other._block_name) {}

ws::Location::~Location() {}

const ws::Location::session_type& ws::Location::get_session() const throw() {
  return _session;
}

const ws::Location::limit_except_vec_type& ws::Location::get_limit_except_vec() const throw() {
  return _limit_except_vec;
}

const ws::Location::return_type& ws::Location::get_return() const throw() {
  return _return;
}

const ws::Location::cgi_map_type& ws::Location::get_cgi_map() const throw() {
  return _cgi_map;
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

void ws::Location::set_session(const session_type &value) {
  _session = value;
}

void ws::Location::add_limit_except(const limit_except_type& value) {
  _limit_except_vec.push_back(value);
}

void ws::Location::set_return(const return_type& value) {
  _return = value;
}

void ws::Location::add_cgi(const cgi_type& value) {
  _cgi_map.insert(value);
}

void ws::Location::set_cgi_map(const cgi_map_type& value) {
  _cgi_map = value;
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
