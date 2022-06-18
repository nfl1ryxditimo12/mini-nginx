#pragma once

#include "Location.hpp"

namespace ws {
class Server {
 private:
  std::vector<std::string> _server_names;
  std::map<std::string, ws::Location> _location;
  std::map<int, std::string> _listen;
  ws::InnerOption _option;

 public:
  Server();
  ~Server();

  void  setServerNames(const std::vector<std::string>& server_names);
  void  setListen(const std::map<int, std::string>& listen);
};
}  // namespace ws
