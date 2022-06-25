#pragma once

#include "ConfParser.hpp"

namespace ws {
  class Configure {
  public:
    typedef ws::Server::listen_type listen_type;
    typedef ws::Server::listen_value_type listen_value_type;

  private:
    ws::ConfParser _parser;
    std::vector<ws::Server> _server;

    Configure& operator=(const Configure& other);
    Configure(const Configure& other);

  public:
    Configure(const std::string& file, const std::string& curr_dir);
    ~Configure();


    const std::vector<ws::Server>& get_server() const throw();
    const listen_type& get_host_list() const;
    //Server& find();
  };
}
