#include "Server.hpp"

ws::Server::Server() {
  // this->_server_names.push_back("_");
  // this->_location.insert(std::pair<std::string, ws::Location>("/", ws::Location()));
  // this->_listen.insert(std::pair<int, std::string>(8000, LOCALHOST));
}

ws::Server::~Server() {}

void  ws::Server::setServerNames(const std::vector<std::string>& server_names) {
  _server_names = server_names;
}
#include <iostream>
void  ws::Server::set_listen(const std::pair<u_int32_t, u_int16_t>& listen) {
  std::cout << listen.first << ", " << listen.second << std::endl;
  _listen.push_back(listen);
}
