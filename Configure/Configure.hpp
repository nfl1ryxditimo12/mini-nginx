#pragma once

#include "ConfParser.hpp"

namespace ws {
class Configure {
private:
  static const ws::ConfParser parser;
  std::vector<ws::Server> server;

  Configure();
  Configure& operator=(const Configure& other);
  Configure(const Configure& other);

public:
  Configure(const std::string& file, const std::string& root_dir);
  ~Configure();
  //Server& find();
};
}
