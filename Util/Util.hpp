#pragma once

#include <limits>
#include <string>

namespace ws {
  std::string get_curr_dir() throw();
  void check_executed_dir();

  unsigned long stoul(
    const std::string& str,
    unsigned long min = 0,
    unsigned long max = std::numeric_limits<unsigned long>::max() - 1
  ) throw();
}
