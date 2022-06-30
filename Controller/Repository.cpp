#include "Repository.hpp"

ws::Repository::Repository() {}

ws::Repository::~Repository() {}

/*getter*/
const ws::Repository::listen_vec_type& ws::Repository::get_listen_vec() const throw() {
  return _listen_vec;
}

const ws::Repository::server_name_vec_type& ws::Repository::get_server_name_vec() const throw() {
  return _server_name_vec;
}

const ws::Repository::location_map_type& ws::Repository::get_location_map() const throw() {
  return _location_map;
}

const ws::Repository::limit_except_vec_type& ws::Repository::get_limit_except_vec() const throw() {
  return _limit_except_vec;
}

const ws::Repository::return_type& ws::Repository::get_return() const throw() {
  return _return;
}

const ws::Repository::cgi_type& ws::Repository::get_cgi() const throw() {
  return _cgi;
}

const ws::Repository::autoindex_type& ws::Repository::get_autoindex() const throw() {
  return _autoindex;
}

const ws::Repository::root_type& ws::Repository::get_root() const throw() {
  return _root;
}

const ws::Repository::index_vec_type& ws::Repository::get_index_vec() const throw() {
  return _index_vec;
}

const ws::Repository::client_max_body_size_type& ws::Repository::get_client_max_body_size() const throw() {
  return _client_max_body_size;
}

const ws::Repository::error_page_map_type& ws::Repository::get_error_page_map() const throw() {
  return _error_page_map;
}

/*setter*/
void ws::Repository::set_listen_vec(const ws::Repository::listen_vec_type& value) {
  _listen_vec = value;
}

void ws::Repository::set_server_name_vec(const ws::Repository::server_name_vec_type& value) {
  _server_name_vec = value;
}

void ws::Repository::set_location_map(const ws::Repository::location_map_type& value) {
  _location_map = value;
}

void ws::Repository::set_limit_except_vec(const ws::Repository::limit_except_vec_type& value) {
  _limit_except_vec = value;
}

void ws::Repository::set_return(const ws::Repository::return_type& value) {
  _return = value;
}

void ws::Repository::set_cgi(const ws::Repository::cgi_type& value) {
  _cgi = value;
}

void ws::Repository::set_autoindex(const ws::Repository::autoindex_type& value) {
  _autoindex = value;
}

void ws::Repository::set_root(const ws::Repository::root_type& value) {
  _root = value;
}

void ws::Repository::set_index_vec(const ws::Repository::index_vec_type& value) {
  _index_vec = value;
}

void ws::Repository::client_max_body_size(const ws::Repository::client_max_body_size_type& value) {
  _client_max_body_size = value;
}

void ws::Repository::error_page_map(const ws::Repository::error_page_map_type& value) {
  _error_page_map = value;
}

void ws::Repository::set_repository(const ws::Server curr_server) {
  
}

void ws::Repository::get_repository(const ws::Configure* _conf, const ws::Request* _request) throw() {
  for (ws::Configure::server_vec_type::const_iterator server_iter = _conf->get_server_vec().begin(); server_iter != _conf->get_server_vec().end(); server_iter++) {
    for (ws::Server::listen_vec_type::const_iterator listen_iter = server_iter->get_listen_vec().begin(); listen_iter != server_iter->get_listen_vec().end(); listen_iter++) {
      if (_request->get_listen() == *listen_iter) {
        ws::Repository::set_repository(*server_iter);
        break;
      }
    }
  }
}
