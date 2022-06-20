#pragma once

#include <sstream>

#include "Server.hpp"

namespace ws {
  class ConfParser {
  private:
    typedef std::map<std::string, void (*)(ws::Server&, const std::string&, std::string::size_type)>
      server_parser_func_map;

    std::string open_file(const std::string& file) const;
    void check_block_header(const std::string& line, const std::string& block_name) const;
    ws::Server parse_server(std::stringstream& buffer, const std::string& curr_dir) const;
    void init_server_parser_func(server_parser_func_map& server_parser_func) const;
    static void parse_listen(ws::Server& server, const std::string& line, std::string::size_type pos);
    static u_int32_t parse_listen_host_ip(const std::string& line, std::string::size_type& pos);
    static u_int16_t parse_listen_port(const std::string& line, std::string::size_type& pos);

    ConfParser(const ConfParser& other);
    ConfParser& operator=(const ConfParser& other);

  public:
    ConfParser();
    ~ConfParser();

    std::vector<ws::Server> parse(const std::string& file, const std::string& curr_dir) const;
  };
}
