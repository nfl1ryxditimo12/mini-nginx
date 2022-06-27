#pragma once

#include <string>
#include <limits.h>

namespace ws {
  std::string get_curr_dir() throw();
  void check_executed_dir();

  std::string::size_type skip_whitespace(const std::string& line, std::string::size_type pos = 0);
  unsigned long stoul(const std::string& str, unsigned long limit = ULONG_MAX);
}
