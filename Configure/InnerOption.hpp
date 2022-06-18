#pragma once

#include <map>
#include <string>
#include <vector>

namespace ws {
class InnerOption {
 private:
  std::map<int, std::string> _error_page;
  std::string _root;
  std::vector<std::string> _index;
  int _client_max_body_size;
  int _directory_flag;

 public:
  InnerOption();
  ~InnerOption();
};
}  // namespace ws
