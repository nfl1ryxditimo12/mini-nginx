#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "Enum.hpp"
#include "Token.hpp"

namespace ws {

  class RequestMessage {
  private:
    /*
    GET == 1, POST == 2, DELETE == 3
    */
    ws::HttpMethod _method;
    std::string _request_uri;
    std::string _query_string;
    std::string _http_version;
    std::map<std::string, std::string> _header;
    std::string _body;
    std::string::size_type  _request_size;

    RequestMessage(const RequestMessage& cls);
    RequestMessage& operator=(const RequestMessage& cls);

    void	parse_request_body(ws::Token& token, std::stringstream& buffer);
    void	parse_request_header(ws::Token& token, std::stringstream& buffer);

  public:
    RequestMessage();
    ~RequestMessage();

    void  parse_request_message(const char* message, int buffer_size);
    void  print_message();
    RequestMessage* get_RequestMessage();
  };
}
