#pragma once

#include <vector>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include <arpa/inet.h>

#include "Configure.hpp"
#include "Token.hpp"
#include "Configure.hpp"
#include "Define.hpp"
#include "Util.hpp"

namespace ws {

  class Repository;

  class Request {
  public:
    typedef void (Request::*header_parse_func) (const std::string&);
    typedef std::map<std::string, header_parse_func> header_parse_map_type;

    typedef ws::Configure::listen_type listen_type;

    typedef std::pair<std::string, std::string> query_pair_type;
    typedef std::map<query_pair_type::first_type, query_pair_type::second_type>  query_map_type;

    typedef std::pair<std::string, std::string> header_pair_type;
    typedef std::map<header_pair_type::first_type, header_pair_type::second_type> header_map_type;

    /*
     * body value type
     * first: request body
     * second: body size
    */
    typedef std::pair<const char*, size_t>               body_value_type;
    typedef std::list<body_value_type>                body_list_type;

  private:

    /* =================================== */
    /*             Util variable           */
    /* =================================== */

    listen_type _listen;
    bool        _eof;

    /* =================================== */
    /*              Start line             */
    /* =================================== */

    std::string _method;
    std::string _request_uri;
    query_map_type  _request_uri_query;
    std::string _http_version; // required HTTP/1.1 protocol

    /* =================================== */
    /*           Request message           */
    /* =================================== */

    header_map_type _request_header;
    body_list_type  _request_body;
    size_t          _request_body_size;

    /* =================================== */
    /*         Request header field        */
    /* =================================== */

    /*
      Transfer-Encoding: chunked 인 경우와 Content-Length 항목이 없는 경우 0
      그 외 Content-Length의 value가 들어감
    */
    std::size_t _content_length;
    std::string _content_type;
    std::string _server_name;
    u_int16_t   _port;
    std::string _connection;
    std::string _transfer_encoding;

    /* =================================== */
    /*           getter variable          */
    /* =================================== */

    /* Request field is "Transfer-Encoding: chunked", true or false */
    bool                    _chunked;

    /* line type is "bytes to send" or "data" */
    bool                    _chunked_line_type;

    bool                    _chunked_eof;

    /* Number of bytes being read */
    size_t                  _chunked_byte;

    size_t                  _client_max_body_size;

    // Header end var for chunked header
    bool                    _is_header;

    int                _request_message_offset;
    size_t             _request_body_offset;

    // buffer to read
    std::stringstream       _buffer;

    /* =================================== */
    /*                 OCCF                */
    /* =================================== */

    Request();
    Request& operator=(const Request& cls);

  public:
    Request(const ws::Configure::listen_type& listen);
    Request(const Request& cls);
    ~Request();

    void  test(); // todo test function

    /* =================================== */
    /*                Getter               */
    /* =================================== */

    std::stringstream& r_buffer() throw();

    /* Getter 더 필요함 변수 확인 필요 */

    bool  get_eof() const throw();
    const std::string& get_method() const throw();
    const std::string& get_uri() const throw();
    const query_map_type& get_uri_query() const throw();
    const std::string& get_version() const throw();

    const header_map_type& get_request_header() const throw();
    body_list_type& get_request_body() throw();
    const size_t& get_request_body_size() const throw();
    const listen_type& get_listen() const throw();

    const std::string::size_type& get_content_length() const throw();
    const std::string& get_content_type() const throw();
    const std::string& get_server_name() const throw();
    const u_int16_t&    get_port() const throw();
    const std::string& get_connection() const throw();
    const std::string& get_transfer_encoding() const throw();

    bool get_is_header() const throw();
    bool get_chunked() const throw();
    bool get_chunked_eof() const throw();
    bool get_chunked_line_type() const throw();
    const size_t& get_chunked_byte() const throw();
    const size_t& get_client_max_body_size() const throw();
    const int& get_request_message_offset() const throw();
    const size_t& get_request_body_offset() const throw();

    /* =================================== */
    /*                Setter               */
    /* =================================== */

    void set_eof(bool) throw();
    void set_method(const std::string&) throw();
    void set_request_uri(const std::string&) throw();
    void set_request_uri_query(const query_pair_type&) throw();
    void set_http_version(const std::string&) throw();

    void set_request_header(const header_pair_type&) throw();
    void set_request_body(const char*, size_t, size_t) throw();
    void set_request_body_size(int size) throw();
    void add_request_body_size(int size) throw();
    void set_listen(const listen_type&) throw();

    void set_content_length(const size_t&) throw();
    void set_content_type(const std::string&) throw();
    void set_server_name(const std::string&) throw();
    void set_port(const u_int16_t&) throw();
    void set_connection(const std::string&) throw();
    void set_transfer_encoding(const std::string&) throw();

    void set_is_header(bool) throw();
    void set_chunked(bool) throw();
    void set_chunked_eof(bool) throw();
    void set_chunked_line_type(bool) throw();
    void set_chunked_byte(const size_t&) throw();
    void add_chunked_byte(const int&) throw();
    void set_client_max_body_size(const size_t&) throw();
    void set_request_message_offset(const size_t&) throw();
    void set_request_body_offset(const size_t&) throw();
    void add_request_message_offset(const int&) throw();
    void add_request_body_offset(const size_t&) throw();
  };
}
