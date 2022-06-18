#pragma once

#include <sstream>

#include "Server.hpp"

namespace ws {
  class ConfParser {
  private:
    std::string open_file(const std::string& file) const;
    void  check_block_header(const std::string& line) const;
    ws::Server& parse_server(std::stringstream& buffer) const throw();

    ConfParser(const ConfParser& other);
    ConfParser& operator=(const ConfParser& other);

  public:
    ConfParser();
    ~ConfParser();

    std::vector<ws::Server> parse(const std::string& file, const std::string& root_dir) const;
  };
}
