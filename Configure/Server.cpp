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

  Server::server_name_type& ws::Server::get_server_name() throw() {
    return _server_name;
  }

  const Server::server_name_type& ws::Server::get_server_name() const throw() {
    return _server_name;
  }

  Server::location_type& ws::Server::get_location() throw() {
    return _location;
  }

  const Server::location_type& ws::Server::get_location() const throw() {
    return _location;
  }

  Server::listen_type& ws::Server::get_listen() throw() {
    return _listen;
  }

  const Server::listen_type& ws::Server::get_listen() const throw() {
    return _listen;
  }

  ws::InnerOption& ws::Server::get_option() throw() {
    return _option;
  }

  const ws::InnerOption& ws::Server::get_option() const throw() {
    return _option;
  }

  void  ws::Server::set_listen(const std::pair<u_int32_t, u_int16_t>& listen) {
    for (std::vector<listen_value_type>::iterator iter = _listen.begin(); iter != _listen.end(); ++iter) {
      if (*iter == listen)
        throw std::invalid_argument("Configure: listen: duplicated value appeared");
    }

    _listen.push_back(listen);
  }

  void  ws::Server::set_server_name(const std::string& server_name) throw() {
    _server_name.push_back(server_name);
  }
}
