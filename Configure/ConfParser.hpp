#pragma once

#include <sstream>

#include "Server.hpp"

namespace ws {
  class ConfParser {
  private:
    std::string open_file(const std::string& file) const;
    std::string check_block_line(const std::string& line) const;
    ws::Server parse_server(std::stringstream& buffer, const std::string& root_dir) const throw();

    ConfParser(const ConfParser& other);
    ConfParser& operator=(const ConfParser& other);

  public:
    ConfParser();
    ~ConfParser();

    std::vector<ws::Server> parse(const std::string& file, const std::string& root_dir) const;
  };
}
