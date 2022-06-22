#pragma once

#include "InnerOption.hpp"

namespace ws {
  class Location {
   private:
    std::map<std::string, bool> _limit_except;
    std::pair<int, std::string> _return;
    std::pair<std::string, std::string> _cgi;
    ws::InnerOption _option;

   public:
    Location();
    ~Location();
  };
}  // namespace ws
