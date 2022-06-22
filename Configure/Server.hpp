#pragma once

#include "Location.hpp"

namespace ws {
class Server {
public:
  typedef std::vector<std::string> server_name_type;
  typedef std::map<std::string, ws::Location> location_type;
  typedef std::vector<std::pair<u_int32_t, u_int16_t> > listen_type;
  typedef listen_type::value_type listen_value_type;

private:
  server_name_type _server_name;
  location_type _location;
  listen_type _listen;
  ws::InnerOption _option;

public:
  Server();
  ~Server();

  server_name_type& get_server_name() throw();
  const server_name_type& get_server_name() const throw();

  location_type& get_location() throw();
  const location_type& get_location() const throw();

  listen_type& get_listen() throw();
  const listen_type& get_listen() const throw();

  ws::InnerOption&  get_option() throw();
  const ws::InnerOption&  get_option() const throw();

  void  set_listen(const listen_value_type& listen);
  void  set_server_name(const std::string& server_name) throw();
};
}  // namespace ws
