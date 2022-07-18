#pragma once

#include "Configure.hpp"
#include "Socket.hpp"

namespace ws {
  class RequestParser {
  private:

    RequestParser(const RequestParser& cls);
    RequestParser& operator=(const RequestParser& cls);
  public:
    RequestParser();
    ~RequestParser();

    int   parse_request_message(const ws::Configure& conf)
  };
}
