#pragma once

#include <sstream>

#include "Server.hpp"
#include "Token.hpp"

namespace ws {
  class ConfParser {
  private:
    typedef void (ConfParser::*server_parser_func_type) (ws::Server&);
    typedef std::map<std::string, server_parser_func_type> server_parser_func_map;
    typedef ws::Server::listen_type listen_type;
    typedef ws::Server::listen_value_type listen_value_type;


    ws::Token _token;
    std::stringstream _buffer;
    const std::string _root_dir;

    std::string open_file(const std::string& file) const;

    ws::Token& rdword();

    void check_block_header(const std::string& block_name);

    ws::Server parse_server();
    void init_server_parser_func(server_parser_func_map& server_parser_func) const;

    void parse_listen(ws::Server& server);
    void parse_listen_host(char delim, listen_value_type& value);
    void parse_listen_port(listen_value_type& value);

    void parse_server_name(ws::Server& server);

    void parse_client_max_body_size(ws::Server& server);

    void parse_autoindex(ws::Server& server);

    void parse_root(ws::Server& server);

    void parse_index(ws::Server& server);

    ConfParser(const ConfParser& other);
    ConfParser& operator=(const ConfParser& other);

  public:
    ConfParser(const std::string& file, const std::string& root_dir);
    ~ConfParser();

    ws::Token& get_token() throw();

    std::stringstream& get_buffer() throw();

    std::vector<ws::Server> parse();
  };
}
