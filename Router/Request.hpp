#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>

// #include "Enum.hpp"
#include "Token.hpp"

namespace ws {
  class Request {
  public:
    typedef std::map<std::string, std::string> header_type;

  private:
    std::string _method;
    std::string _request_uri;
    // std::string _request_uri_query = "";
    std::string _http_version; //protocol
    header_type _request_header;
    std::string _request_body;
    // std::string::size_type  _request_size;

    Request& operator=(const Request& other);
    Request(const Request& other);

  public:
    Request();
    ~Request();

    void  parse_request_message(const char* message);
    void  print_message();

    //getter
    std::string get_method() const throw();
    std::string get_uri() const throw();
    std::string get_version() const throw();
    header_type get_request_header() const throw();
    std::string get_request_body() const throw();
    // std::string::size_type get_request_size() const throw();
  };
}