#pragma once

#include <sstream>

#include "Configure.hpp"
#include "Socket.hpp"
#include "Define.hpp"

namespace ws {
  class RequestParser {

  public:
    typedef void (RequestParser::*header_parse_func) (const std::string&);
    typedef std::map<std::string, header_parse_func> header_parse_map_type;

    typedef ws::Socket::client_value_type client_value_type;

    typedef ws::Request::header_map_type       header_map_type;

    typedef ws::Request::query_map_type        query_map_type;

  private:

    /* ================================== */
    /*          Default variable          */
    /* ================================== */

    const ws::Configure*  _conf;
    header_parse_map_type _header_parser;

    /* ================================== */
    /*           Parser variable          */
    /* ================================== */

    int                 _read_size;
    const char*         _message;
    ws::Token           _token;
    std::stringstream*  _buffer;

    /* ================================== */
    /*             Client data            */
    /* ================================== */

    bool*         _fatal;
    unsigned int* _status;
    Request*      _request;
    Repository*   _repository;


    RequestParser(const RequestParser&);
    RequestParser& operator=(const RequestParser&);

    void  init_request_parser(client_value_type&, const char*, const int);
    void  end_request_parser();

    void  parse_request_uri(const std::string&);
    bool  parse_request_start_line();
    void  parse_request_header();
    void  parse_request_body();
    void  parse_request_chunked_body();
    void  parse_request_chunked_start_line(); // todo delete?
    void  parse_request_chunked_data_line();

    void  parse_host(const std::string&);
    void  parse_connection(const std::string&);
    void  parse_content_length(const std::string&);
    void  parse_content_type(const std::string&);
    void  parse_transfer_encoding(const std::string&);

    ws::Token& rdline(char);
    ws::Token& rd_http_line();

  public:
    RequestParser();
    ~RequestParser();

    void   init_conf(const ws::Configure& conf);
    void   parse_request_message(client_value_type&, const char*, const int);
  };
}
