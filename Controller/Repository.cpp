#include "Repository.hpp"
#include "Request.hpp"

ws::Repository::Repository(const ws::Server* curr_server, const ws::Request& request) {
  /*set server*/
  _listen = &(request.get_listen());
  _server_name = &(request.get_request_header().find("Host")->second);
  // _server_name = &(request.get_server_name());
  
  /*set location*/
  const ws::Location* curr_location = curr_server->find_location(request.get_uri());

  if (curr_location != NULL) {
    _limit_except_vec = &(curr_location->get_limit_except_vec());
    _return = &(curr_location->get_return());
    _cgi = &(curr_location->get_cgi());
  /*set option*/
    ws::Repository::set_option(curr_location->get_option());
  }
  ws::Repository::set_option(curr_server->get_option());
}

void ws::Repository::set_option(const ws::InnerOption& option) {
  _autoindex = &(option.get_autoindex());
  _root = &(option.get_root());
  _index_vec = &(option.get_index_vec());
  _client_max_body_size = &(option.get_client_max_body_size());
  _error_page_map = &(option.get_error_page_map());
}

ws::Repository::~Repository() {}

/*getter*/
const ws::Repository::listen_type& ws::Repository::get_listen() const throw() {
  return *_listen;
}

const ws::Repository::server_name_type& ws::Repository::get_server_name() const throw() {
  return *_server_name;
}

const ws::Repository::location_dir_type& ws::Repository::get_dir() const throw() {
  return *_location_dir;
}

const ws::Repository::limit_except_vec_type& ws::Repository::get_limit_except_vec() const throw() {
  return *_limit_except_vec;
}

const ws::Repository::return_type& ws::Repository::get_return() const throw() {
  return *_return;
}

const ws::Repository::cgi_type& ws::Repository::get_cgi() const throw() {
  return *_cgi;
}

const ws::Repository::autoindex_type& ws::Repository::get_autoindex() const throw() {
  return *_autoindex;
}

const ws::Repository::root_type& ws::Repository::get_root() const throw() {
  return *_root;
}

const ws::Repository::index_vec_type& ws::Repository::get_index_vec() const throw() {
  return *_index_vec;
}

const ws::Repository::client_max_body_size_type& ws::Repository::get_client_max_body_size() const throw() {
  return *_client_max_body_size;
}

const ws::Repository::error_page_map_type& ws::Repository::get_error_page_map() const throw() {
  return *_error_page_map;
}
