#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>

// #include "Enum.hpp"
#include "Token.hpp"

namespace ws {
  class RequestMessage {
  public:
    typedef std::map<std::string, std::string> header_type;

  private:
    std::string _method;
    std::string _request_uri;
    std::string _http_version;
    header_type _header;
    std::string _body;
    std::string::size_type  _request_size;

    RequestMessage& operator=(const RequestMessage& other);
    RequestMessage(const RequestMessage& other);

  public:
    RequestMessage();
    ~RequestMessage();

    void  parse_request_message(const char* message, int buffer_size);
    void  print_message();
  };
}