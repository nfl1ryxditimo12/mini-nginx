#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>

// #include "Enum.hpp"
#include "Token.hpp"
#include "Configure.hpp"

namespace ws {
  class Request {
  public:
    typedef std::map<std::string, std::string> header_type;
    typedef ws::Configure::listen_type listen_type;

  private:
    ws::Configure::listen_type _listen;
    std::string _method;
    std::string _request_uri;
    // std::string _request_uri_query = "";
    std::string _http_version; //protocol
    header_type _request_header;
    std::string _request_body;
    // std::string::size_type  _request_size;
    listen_type _listen;


    Request();
    Request& operator=(const Request& cls);

  public:
    Request(ws::Configure::listen_type listen);
    Request(const Request& cls);
    ~Request();

    void  parse_request_message(const char* message);
    void  print_message();

    //getter
    const std::string& get_method() const throw();
    const std::string& get_uri() const throw();
    const std::string& get_version() const throw();
    const header_type& get_request_header() const throw();
    const std::string& get_request_body() const throw();
    const listen_type& get_listen() const throw();
    // std::string::size_type get_request_size() const throw();
  };
}
