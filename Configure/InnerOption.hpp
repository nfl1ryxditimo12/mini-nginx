#pragma once

#include <map>
#include <string>
#include <vector>

namespace ws {
class InnerOption {
public:
  typedef std::map<int, std::string> error_page_type;
  typedef std::string root_type;
  typedef std::vector<int> index_type;

private:
  error_page_type _error_page;
  root_type _root;
  index_type _index;
  unsigned long _client_max_body_size;
  int _directory_list;

public:
  InnerOption();
  ~InnerOption();

  void set_client_max_body_size(unsigned long client_max_body_size) throw();
  void set_directory_list(const std::string& value);
  void set_root(const std::string& value);
};
}  // namespace ws
