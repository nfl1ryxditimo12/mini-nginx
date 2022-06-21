#pragma once

#include <sstream>

#include "Server.hpp"
#include "Token.hpp"

namespace ws {
  class ConfParser {
  private:
    typedef std::map<std::string, void (*)(ws::Server&, ws::Token&, std::stringstream&)>
      server_parser_func_map;
    typedef ws::Server::listen_value_type listen_value_type;

    std::string open_file(const std::string& file) const;
    void check_block_header(ws::Token& token, std::stringstream& buffer, const std::string& block_name) const;
    ws::Server parse_server(ws::Token& token, std::stringstream& buffer, const std::string& curr_dir) const;
    void init_server_parser_func(server_parser_func_map& server_parser_func) const;

    static void parse_listen(ws::Server& server, ws::Token& token, std::stringstream& buffer);
    static void parse_listen_host(ws::Token& token, char delim, listen_value_type& value);
    static void parse_listen_port(ws::Token& token, listen_value_type& value);

    static void parse_server_name(ws::Server& server, ws::Token& token, std::stringstream& buffer);

    static void parse_client_max_body_size(ws::Server& server, ws::Token& token, std::stringstream& buffer);

    static void parse_directory_list(ws::Server& server, ws::Token& token, std::stringstream& buffer);

    ConfParser(const ConfParser& other);
    ConfParser& operator=(const ConfParser& other);

  public:
    ConfParser();
    ~ConfParser();

    std::vector<ws::Server> parse(const std::string& file, const std::string& curr_dir) const;
  };
}
