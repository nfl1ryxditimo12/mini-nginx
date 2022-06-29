#pragma once

#include <set>
#include <sstream>

#include "Configure.hpp"
#include "Server.hpp"
#include "Token.hpp"

namespace ws {
  class ConfParser {
  private:
    typedef std::vector<ws::Server> server_vec_type;
    typedef ws::Server::listen_vec_type listen_vec_type;
    typedef ws::Server::listen_type listen_type;

    typedef ws::Server::location_map_type location_map_type;
    typedef ws::Server::location_pair_type location_pair_type;
    typedef ws::Location::limit_except_type limit_except_type;
    typedef ws::Location::limit_except_vec_type limit_except_vec_type;

    typedef ws::Server::index_type index_type;
    typedef ws::Server::index_vec_type index_vec_type;

    // types for parse function pointer map
    typedef void (ConfParser::*server_parser_func_type) (ws::Server&);
    typedef std::map<std::string, server_parser_func_type> server_parser_func_map;
    typedef server_parser_func_map::iterator server_parser_iterator;

    typedef void (ConfParser::*location_parser_func_type) (ws::Location&);
    typedef std::map<std::string, location_parser_func_type> location_parser_func_map;
    typedef location_parser_func_map::iterator location_parser_iterator;

    typedef void (ConfParser::*option_parser_func_type) (ws::InnerOption&);
    typedef std::map<std::string, option_parser_func_type> option_parser_func_map;
    typedef option_parser_func_map::iterator option_parser_iterator;

    ws::Token _token;
    std::stringstream _buffer;
    const std::string _root_dir;
    server_parser_func_map _server_parser;
    location_parser_func_map _location_parser;
    option_parser_func_map _option_parser;
    std::set<listen_type> _listen_checker;

    std::string read_file(const std::string& file) const;

    ws::Token& rdword();

    void init_server_parser();
    void init_location_parser();
    void init_option_parser();

    void check_server_header();
    void check_location_header(std::string& dir, ws::Location& location);
    bool check_block_end();

    void parse_server(ws::Server& server);
    void parse_location(ws::Location& location);

    void parse_listen(ws::Server& server);
    void parse_listen_host(char delim, listen_type& value);
    void parse_listen_port(listen_type& value);

    void parse_server_name(ws::Server& server);

    void parse_limit_except(ws::Location&);
    std::string get_method(const std::string& method) const;
    void parse_return(ws::Location&);

    void parse_autoindex(ws::InnerOption& inner);
    void parse_root(ws::InnerOption& inner);
    void parse_index(ws::InnerOption& inner);
    void parse_client_max_body_size(ws::InnerOption& inner);
    void parse_error_page(ws::InnerOption& inner);
    int parse_error_code() const;

    void set_default_server(ws::Server& server) const;
    void set_default_location(ws::Server& server, location_map_type& location_map) const;
    void delete_duplicated_listen(ws::Server& server) const;
    void update_listen_checker(const listen_vec_type& listen_vec);

    ConfParser(const ConfParser& other);
    ConfParser& operator=(const ConfParser& other);

  public:
    ConfParser(const std::string& file, const std::string& root_dir);
    ~ConfParser();

    ws::Token& get_token() throw();

    std::stringstream& get_buffer() throw();

    void parse(ws::Configure& conf);
  };
}
