#pragma once

#include "Socket.hpp"

namespace ws {
  class Controller {
  public:
    typedef ws::Socket::client_value_type client_value_type;
    typedef std::string (Controller::*process_method)(const client_value_type& client);
    typedef std::map<std::string, process_method> process_map_type;

  private:
    process_map_type _process_map;

    bool is_error_status(unsigned int stat) const throw();
    std::string get_error_response(const client_value_type& client);

    std::string process_get(const client_value_type& client);
    std::string process_post(const client_value_type& client);
    std::string process_delete(const client_value_type& client);

    Controller(const Controller& other);
    Controller& operator=(const Controller& other);

  public:
    Controller();
    ~Controller();

    std::string get_response(const client_value_type& client);
  };
}
