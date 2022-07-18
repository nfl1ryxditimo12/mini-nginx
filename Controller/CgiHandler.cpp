#include "CgiHandler.hpp"

#include <cstdlib>
#include <cstring>

extern char** environ;

ws::CgiHandler::CgiHandler(ws::Kernel& kernel, int client_fd, client_value_type& client) throw()
  : _kernel(kernel), _client_fd(client_fd), _client_data(client) {}

ws::CgiHandler::~CgiHandler() {
  close(_fpipe[0]);
  close(_fpipe[1]);
  close(_bpipe[0]);
  close(_bpipe[1]);
}

bool ws::CgiHandler::set_cgi_env(const char* method, const char* path) {
  return (
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1) < 0
    || setenv("REQUEST_METHOD", method, 1) < 0
    || setenv("PATH_INFO", path, 1)
  );
}

bool ws::CgiHandler::init_pipe(int fpipe[2], int bpipe[2]) {
  bool ret = !(pipe(fpipe) || pipe(bpipe));
  _client_data.repository.set_fpipe(_fpipe[1]);
  _client_data.repository.set_bpipe(_bpipe[0]);

  return ret;
}

pid_t ws::CgiHandler::init_child(int fpipe[2], int bpipe[2], const char* cgi_path) {
  pid_t pid = fork();

  if (!pid) {
    int fatal = false;

    close(fpipe[1]);
    fatal |= (!dup2(fpipe[0], STDIN_FILENO));
    close(fpipe[0]);

    close(bpipe[0]);
    fatal |= (!dup2(bpipe[1], STDOUT_FILENO));
    close(bpipe[1]);

    char* argv[2];
    argv[0] = strdup(cgi_path);
    argv[1] = NULL;

    if (fatal)
      exit(EXIT_FAILURE);

    execve(cgi_path, argv, environ);

    exit(EXIT_FAILURE);
  }

  close(fpipe[0]);
  close(bpipe[1]);

  return pid;
}

bool ws::CgiHandler::send_cgi() {
  _kernel.add_write_event(_client_fd, Socket::write_pipe);

  return false;
}

bool ws::CgiHandler::recv_cgi() {

  return false;
}

bool ws::CgiHandler::send_client() {
  return false;
}
