#pragma once

#include "InnerOption.hpp"

namespace ws {
enum Method { GET, POST, DELETE };

class Location {
 private:
  std::vector<ws::Method>             _limit_except;
  std::pair<int, std::string>         _return;
  std::pair<std::string, std::string> _cgi;
  ws::InnerOption                 _option;

 public:
  Location();
  ~Location();
};
}  // namespace ws
