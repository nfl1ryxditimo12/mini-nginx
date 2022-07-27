#pragma once

#include <limits>
#include <string>
#include <cmath>

namespace ws {
  class Util {
  private:
    static const std::string _hash;
    static const std::string _root_dir;

    static std::string set_curr_dir();

    Util();
    Util(const Util& other);
    Util& operator=(const Util& other);
    ~Util();

  public:
    static const std::string& get_root_dir();
    static void check_executed_dir();

    static unsigned long stoul(
      const std::string& str,
      unsigned long max = std::numeric_limits<unsigned long>::max() - 1,
      unsigned long min = 0,
      const std::string& base_str = "0123456789"
    ) throw();

    static std::string mime_type(const std::string&);

    static std::string ultos(unsigned long n) throw();

    static bool is_valid_server_name(const std::string& str);

    static std::string parse_relative_path(const std::string& str);

    static bool is_eof(int fd) throw();

    static std::string format_number(size_t num);
  };
}
