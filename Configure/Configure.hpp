#pragma once

#include "Server.hpp"

namespace ws {
  class Configure {
  public:
    typedef std::vector<ws::Server> server_vec_type;

    typedef ws::Server::listen_type listen_type;
    typedef ws::Server::listen_vec_type listen_vec_type;

  private:
    server_vec_type _server_vec;

    Configure& operator=(const Configure& other);
    Configure(const Configure& other);

  public:
    Configure();
    ~Configure();

    const server_vec_type& get_server_vec() const throw();

    void set_server_vec(const server_vec_type& value);

    listen_vec_type get_host_list() const;
  };
}
