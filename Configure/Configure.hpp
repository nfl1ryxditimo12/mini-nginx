#pragma once

#include "ConfParser.hpp"

namespace ws {
  class Configure {
  private:
    ws::ConfParser _parser;
    std::vector<ws::Server> _server;

    Configure& operator=(const Configure& other);
    Configure(const Configure& other);

  public:
    Configure(const std::string& file, const std::string& curr_dir);
    ~Configure();


    const std::vector<ws::Server>& get_server() const throw();
    //Server& find();
  };
}
