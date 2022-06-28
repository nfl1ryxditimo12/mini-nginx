#include "Server.hpp"

#include <arpa/inet.h>
#include <stdexcept>

ws::Server::Server() {
  // this->_server_names.push_back("_");
  // this->_location_map.insert(std::pair<std::string, ws::Location>("/", ws::Location()));
  // this->_listen_vec.insert(std::pair<int, std::string>(8000, LOCALHOST));
}

ws::Server::~Server() {}

const ws::Server::listen_vec_type& ws::Server::get_listen_vec() const throw() {
  return _listen_vec;
}

const ws::Server::server_name_vec_type& ws::Server::get_server_name_vec() const throw() {
  return _server_name_vec;
}

const ws::Server::location_map_type& ws::Server::get_location_map() const throw() {
  return _location_map;
}

const ws::InnerOption& ws::Server::get_option() const throw() {
  return _option;
}

const ws::Server::autoindex_type& ws::Server::get_autoindex() const throw() {
  return _option.get_autoindex();
}

const ws::Server::root_type& ws::Server::get_root() const throw() {
  return _option.get_root();
}

const ws::Server::index_vec_type& ws::Server::get_index_vec() const throw() {
  return _option.get_index_vec();
}

const ws::Server::client_max_body_size_type& ws::Server::get_client_max_body_size() const throw() {
  return _option.get_client_max_body_size();
}

const ws::Server::error_page_map_type& ws::Server::get_error_page_map() const throw() {
  return _option.get_error_page_map();
}

void ws::Server::set_listen_vec(const listen_type& value) {
  for (listen_vec_type::iterator iter = _listen_vec.begin(); iter != _listen_vec.end(); ++iter) {
    if (*iter == value)
      throw std::invalid_argument("Configure: listen: duplicated value appeared");
  }

  _listen_vec.push_back(value);
}

void ws::Server::set_server_name_vec(const server_name_type& value) {
  _server_name_vec.push_back(value);
}

void ws::Server::set_location_map(const location_map_type& value) {
  _location_map = value;
}

void ws::Server::set_option(const ws::InnerOption& value) {
  _option = value;
}
