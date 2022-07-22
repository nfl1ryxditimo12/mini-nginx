#pragma once

#include <unistd.h>

#include "Kernel.hpp"

namespace ws {
  class CgiHandler {
  private:
    int _fpipe[2];
    int _bpipe[2];
    bool _eof;

    bool init_pipe() throw();
    static bool set_cgi_env(const char* method, const char* path_info);
    pid_t init_child(const char* cgi_path, pid_t& pid);

    CgiHandler& operator=(const CgiHandler& other);

  public:
    CgiHandler() throw();
    CgiHandler(const CgiHandler& other);
    ~CgiHandler();

    const int* get_fpipe() const throw();
    const int* get_bpipe() const throw();
    int get_eof() const throw();

    void set_eof(bool value);

    pid_t run_cgi(const char* method, const char* path_info, const char* cgi_path, ws::Kernel& kernel);
  };
}
