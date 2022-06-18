#include "Server.hpp"

#define LOCALHOST "127.0.0.1"

ws::Server::Server() {
  this->_server_names.push_back("_");
  this->_location.insert(
      std::pair<std::string, ws::Location>("/", ws::Location()));
  this->_listen.insert(std::pair<int, std::string>(8000, LOCALHOST));
}

ws::Server::~Server() {}
