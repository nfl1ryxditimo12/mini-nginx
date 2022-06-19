#pragma once

#include <sstream>

#include "Server.hpp"

namespace ws {
  class ConfParser {
  private:
    std::string open_file(const std::string& file) const;
    void check_block_header(const std::string& line, const std::string& block_name) const;
    ws::Server parse_server(std::stringstream& buffer, const std::string& curr_dir) const;

    ConfParser(const ConfParser& other);
    ConfParser& operator=(const ConfParser& other);

  public:
    ConfParser();
    ~ConfParser();

    std::vector<ws::Server> parse(const std::string& file, const std::string& curr_dir) const;
  };
}
