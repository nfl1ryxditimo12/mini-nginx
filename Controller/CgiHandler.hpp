#pragma once

#include <unistd.h>

#include "Socket.hpp"

namespace ws {
  class CgiHandler {
  public:
    typedef ws::Socket::client_value_type client_value_type;
    typedef ws::Socket::client_map_type client_map_type;

  private:
    int _fpipe[2];
    int _bpipe[2];
    ws::Kernel& _kernel;
    int _client_fd;
    client_value_type& _client_data;

    CgiHandler(const CgiHandler& other);
    CgiHandler& operator=(const CgiHandler& other);

  public:
    CgiHandler(ws::Kernel& kernel, int client_fd, client_value_type& client) throw();
    ~CgiHandler();

    // all boolean functions return false if fail
    bool set_cgi_env(const char* method, const char* path);
    bool init_pipe(int fpipe[2], int bpipe[2]);
    pid_t init_child(int fpipe[2], int bpipe[2], const char* cgi_path);
    bool send_cgi();
    bool recv_cgi();
    bool send_client();
  };
}
