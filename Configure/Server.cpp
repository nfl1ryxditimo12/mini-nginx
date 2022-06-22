#include "Server.hpp"

#include <arpa/inet.h>
#include <stdexcept>
#include <iostream> //todo

namespace ws {
  ws::Server::Server() {
    // this->_server_names.push_back("_");
    // this->_location.insert(std::pair<std::string, ws::Location>("/", ws::Location()));
    // this->_listen.insert(std::pair<int, std::string>(8000, LOCALHOST));
  }

  ws::Server::~Server() {}

  const Server::listen_type& ws::Server::get_listen() const throw() {
    return _listen;
  }

  const Server::server_name_type& ws::Server::get_server_name() const throw() {
    return _server_name;
  }

  const Server::location_type& ws::Server::get_location() const throw() {
    return _location;
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

  const ws::Server::index_type& ws::Server::get_index() const throw() {
    return _option.get_index();
  }

  const ws::Server::client_max_body_size_type& ws::Server::get_client_max_body_size() const throw() {
    return _option.get_client_max_body_size();
  }

  const ws::Server::error_page_type& ws::Server::get_error_page() const throw() {
    return _option.get_error_page();
  }

  void ws::Server::set_listen(const listen_value_type& value) {
    for (std::vector<listen_value_type>::iterator iter = _listen.begin(); iter != _listen.end(); ++iter) {
      if (*iter == value)
        throw std::invalid_argument("Configure: listen: duplicated value appeared");
    }

    _listen.push_back(value);
  }

  void ws::Server::set_server_name(const server_name_value_type& value) throw() {
    _server_name.push_back(value);
  }

  void ws::Server::set_autoindex(const autoindex_type& value) throw() {
    _option.set_autoindex(value);
  }

  void ws::Server::set_root(const root_type& value) throw() {
    _option.set_root(value);
  }

  void ws::Server::set_index(const index_type& value) throw() {
    _option.set_index(value);
  }

  void ws::Server::set_client_max_body_size(const client_max_body_size_type& value) throw() {
    _option.set_client_max_body_size(value);
  }

  void ws::Server::set_error_page(const error_page_type& value) throw() {
    _option.set_error_page(value);
  }
}
