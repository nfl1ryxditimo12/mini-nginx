#include "Repository.hpp"
#include "Request.hpp"

ws::Repository::Repository(const ws::Server& curr_server, const ws::Request& request) {
  /*set server*/
  _listen = ;
  _server_name = ;
  /*set location*/
  const location_map_type& location_map = curr_server.get_location_map();
  location_map_type::const_iterator curr_location = location_map.find(request.get_uri());

  if (curr_location != location_map.end()) {
    _limit_except_vec = curr_location->second.get_limit_except_vec();
    _return = curr_location->second.get_return();
    _cgi = curr_location->second.get_cgi();
  } else {
    // 404
  }
  /*set option*/
  _autoindex = curr_location->second.get_autoindex();
  _root = curr_location->second.get_root();
  _index_vec = curr_location->second.get_index_vec();
  _client_max_body_size = curr_location->second.get_client_max_body_size();
  _error_page_map = curr_location->second.get_error_page_map();
}

ws::Repository::~Repository() {}

/*getter*/
const ws::Repository::listen_type& ws::Repository::get_listen() const throw() {
  return _listen;
}

const ws::Repository::server_name_type& ws::Repository::get_server_name() const throw() {
  return _server_name;
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
