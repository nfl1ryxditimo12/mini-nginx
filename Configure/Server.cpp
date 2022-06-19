#include "Server.hpp"

#define LOCALHOST "127.0.0.1"

ws::Server::Server() {
  this->_server_names.push_back("_");
  this->_location.insert(std::pair<std::string, ws::Location>("/", ws::Location()));
  this->_listen.insert(std::pair<int, std::string>(8000, LOCALHOST));
}

ws::Server::~Server() {}

void  ws::Server::setServerNames(const std::vector<std::string>& server_names) {
  _server_names = server_names;
}

void  ws::Server::setListen(const std::map<int, std::string>& listen) {
  _listen = listen;
}
