#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "Enum.hpp"

namespace ws {

  class RequestMessage {
  private:
    /*
    GET == 1, POST == 2, DELETE == 3
    */
    ws::HttpMethod _method;
    std::string _request_uri;
    std::map<std::string, std::string> _header;
    std::string _body;

    RequestMessage(const RequestMessage& cls);
    RequestMessage& operator=(const RequestMessage& cls);

  public:
    RequestMessage();
    ~RequestMessage();

    void  parse_request_message(const std::string& message);
    void  print_message();
  };
}