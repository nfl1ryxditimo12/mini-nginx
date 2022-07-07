#pragma once

#include <limits>
#include <string>

namespace ws {
  std::string get_curr_dir() throw();
  void check_executed_dir();

  unsigned long stoul(
    const std::string& str,
    unsigned long max = std::numeric_limits<unsigned long>::max() - 1,
    unsigned long min = 0
  ) throw();

  unsigned long hextoul(
    const std::string& str,
    unsigned long max = std::numeric_limits<unsigned long>::max() - 1,
    unsigned long min = 0
  ) throw();

  std::string ultoa(unsigned long n);

  bool is_valid_server_name(const std::string& str);

  class Convertor;
}
