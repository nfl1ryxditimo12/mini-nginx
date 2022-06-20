#pragma once

#include "Location.hpp"

namespace ws {
class Server {
 private:
  typedef std::pair<u_int32_t, u_int16_t> listen_value_type;

  std::vector<std::string> _server_names;
  std::map<std::string, ws::Location> _location;
  std::vector<listen_value_type> _listen;
  ws::InnerOption _option;

 public:
  Server();
  ~Server();

  void  setServerNames(const std::vector<std::string>& server_names);
  void  set_listen(const listen_value_type& listen);
};
}  // namespace ws
