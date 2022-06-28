#pragma once

#include "ConfParser.hpp"

namespace ws {
  class Configure {
  public:
    typedef std::vector<ws::Server> server_vec_type;

    typedef ws::Server::listen_type listen_type;
    typedef ws::Server::listen_vec_type listen_vec_type;

  private:
    ws::ConfParser _parser;
    server_vec_type _server_vec;

    Configure& operator=(const Configure& other);
    Configure(const Configure& other);

  public:
    Configure(const std::string& file, const std::string& curr_dir);
    ~Configure();

    const server_vec_type& get_server_vec() const throw();

    listen_vec_type get_host_list() const;
    //Server& find();
  };
}
