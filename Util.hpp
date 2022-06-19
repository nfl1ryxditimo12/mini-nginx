#pragma once

#include <string>

namespace ws {
  class Util {
  private:
    static char* _curr_dir;

    Util(const Util& other);
    Util& operator=(const Util& other);

  public:
    Util();
    ~Util();

    static void check_executed_dir();
    static std::string get_dir() throw();
  };
}
