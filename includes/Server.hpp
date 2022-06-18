#ifndef SERVER_HPP
#define SERVER_HPP

#include "ConfigureOption.hpp"
#include "Location.hpp"

namespace ws {
class Server {
 private:
  std::vector<std::string>            _server_names;
  std::map<std::string, ws::Location> _location;
  std::map<int, std::string>          _listen;
  ws::ConfigureOption                 _option;

 public:
  Server();
  ~Server();
};
}  // namespace ws

#endif
