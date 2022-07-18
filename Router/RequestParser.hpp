#pragma once

#include "Configure.hpp"
#include "Socket.hpp"

namespace ws {
  class RequestParser {

  public:
    typedef void (RequestParser::*header_parse_func) (const std::string&);
    typedef std::map<std::string, header_parse_func> header_parse_map_type;

    typedef ws::Socket::client_value_type client_value_type;

  private:

    header_parse_map_type _header_parser;

    bool*         _fatal;
    unsigned int* _status;
    Request*      _request;
    Repository*   _repository;


    RequestParser(const RequestParser& cls);
    RequestParser& operator=(const RequestParser& cls);

    void  init_client_data(client_value_type& client_data);

    void  parse_request_uri(std::string uri);
    bool  parse_request_start_line();
    void  parse_request_header();
    void  parse_request_body();
    void  parse_request_chunked_body();
    void  parse_request_chunked_start_line();
    void  parse_request_chunked_data_line();

    void  parse_host(const std::string& value);
    void  parse_connection(const std::string& value);
    void  parse_content_length(const std::string& value);
    void  parse_content_type(const std::string& value);
    void  parse_transfer_encoding(const std::string& value);


  public:
    RequestParser();
    ~RequestParser();

    int   parse_request_message(const ws::Configure& conf, client_value_type& client_data, const char* message, const int read_size);
  };
}
