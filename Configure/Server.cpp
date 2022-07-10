#include "Server.hpp"

#include <arpa/inet.h>
#include <stdexcept>

#include "Util.hpp"

ws::Server::Server() {
  // this->_server_names.push_back("_");
  // this->_location_map.insert(std::pair<std::string, ws::Location>("/", ws::Location()));
  // this->_listen_vec.insert(std::pair<int, std::string>(8000, LOCALHOST));
}

ws::Server::Server(const Server& other)
  : _listen_vec(other._listen_vec),
  _server_name_vec(other._server_name_vec),
  _location_map(other._location_map),
  _option(other._option) {}

ws::Server& ws::Server::operator=(const Server& other) {
  Server temp(other);
  std::swap(*this, temp);
  return *this;
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

const ws::Server::index_set_type& ws::Server::get_index_set() const throw() {
  return _option.get_index_set();
}

const ws::Server::client_max_body_size_type& ws::Server::get_client_max_body_size() const throw() {
  return _option.get_client_max_body_size();
}

const ws::Server::error_page_map_type& ws::Server::get_error_page_map() const throw() {
  return _option.get_error_page_map();
}

void ws::Server::add_listen(const listen_type& value) {
  for (listen_vec_type::iterator iter = _listen_vec.begin(); iter != _listen_vec.end(); ++iter) {
    if (*iter == value)
      throw std::invalid_argument("Configure: listen: duplicated value appeared");
  }

  _listen_vec.push_back(value);
}

void ws::Server::set_listen_vec(const listen_vec_type& value) {
  _listen_vec = value;
}

void ws::Server::add_server_name(const server_name_type& value) {
  _server_name_vec.push_back(value);
}

void ws::Server::set_location_map(const location_map_type& value) {
  _location_map = value;
}

void ws::Server::set_option(const ws::InnerOption& value) {
  _option = value;
}

void ws::Server::set_autoindex(const autoindex_type& value) {
  _option.set_autoindex(value);
}

void ws::Server::set_root(const root_type& value) {
  _option.set_root(value);
}

void ws::Server::add_index(const index_type& value) {
  _option.add_index(value);
}

void ws::Server::set_client_max_body_size(const client_max_body_size_type& value) {
  _option.set_client_max_body_size(value);
}
#include <iostream> // todo
const ws::Location& ws::Server::find_location(location_pair_type::first_type dir) const throw() {
  location_map_type::const_iterator result;

  while (dir.length()) {
    std::cout << dir << std::endl; // todo
    result = _location_map.find(dir);
    if (result != _location_map.end())
      return result->second;
    dir.erase(dir.find_last_of('/'));
  }

  return _location_map.find("/")->second;
}
