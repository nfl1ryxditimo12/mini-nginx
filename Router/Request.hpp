#pragma once

#include <vector>
#include <map>
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

    typedef std::map<std::string, std::string> query_type;

    typedef std::map<std::string, std::string> header_type;

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
    query_type  _request_uri_query;
    std::string _http_version; // required HTTP/1.1 protocol

    /* =================================== */
    /*           Request message           */
    /* =================================== */

    header_type _request_header;
    std::string _request_body;

    /* =================================== */
    /*         Request header field        */
    /* =================================== */

    /*
      Transfer-Encoding: chunked 인 경우와 Content-Length 항목이 없는 경우 0
      그 외 Content-Length의 value가 들어감
    */
    size_t      _content_length;
    std::string _server_name;
    std::string _connection;
    std::string _transfer_encoding;

    /* =================================== */
    /*         Non-getter variable         */
    /* =================================== */

    /* parser to want header field */
    header_parse_map_type _header_parser;
    /*
      status
      case   0: Right request message
      case > 0: Wrong request message
    */
    int                     _status;

    /* Request field is "Transfer-Encoding: chunked", true or false */
    bool                    _chunked;

    /* line type is "bytes to send" or "data" */
    int                     _chunked_line_type;

    /* Number of bytes being read */
    size_t                  _chunked_byte;

    size_t                  _client_max_body_size;

    /* =================================== */
    /*                 OCCF                */
    /* =================================== */

    Request();
    Request(const Request& cls);
    Request& operator=(const Request& cls);

    /* =================================== */
    /*            Request parser           */
    /* =================================== */

    void  parse_request_uri(ws::Token& token, std::string uri);
    void  parse_request_header(ws::Token& token, std::stringstream& buffer);
    void  parse_request_body(std::stringstream& buffer);
    void  parse_request_chunked_body(ws::Token& token, std::stringstream& buffer);

    /* =================================== */
    /*     Request header field parser     */
    /* =================================== */

    /*
      Transfer-Encoding
      Host
      Connection
      Content-Length
      등등 추가될 수 있음
    */
    void  parse_host(const std::string& value);
    void  parse_connection(const std::string& value);
    void  parse_content_length(const std::string& value);
    void  parse_transfer_encoding(const std::string& value);

    /* =================================== */
    /*        Else private function        */
    /* =================================== */

    void  insert_require_header_field();

  public:
    Request(const ws::Configure::listen_type& listen);
    ~Request();

    int   parse_request_message(const ws::Configure* conf, ws::Repository* repository, const std::string message);
    void  clear();
    void  test();

    /* =================================== */
    /*                Getter               */
    /* =================================== */

    /* Getter 더 필요함 변수 확인 필요 */
    bool eof() const throw();
    const std::string& get_method() const throw();
    const std::string& get_uri() const throw();
    const query_type& get_uri_query() const throw();
    const std::string& get_version() const throw();

    const header_type& get_request_header() const throw();
    const std::string& get_request_body() const throw();
    const listen_type& get_listen() const throw();

    const std::string::size_type& get_content_length() const throw();
    const std::string& get_server_name() const throw();
    const std::string& get_connection() const throw();
    const std::string& get_transfer_encoding() const throw();
  };
}
