#pragma once

#include "Socket.hpp"
#include "HeaderGenerator.hpp"

namespace ws {
  class Response {
  public:
    typedef ws::Socket::client_value_type client_value_type;

  private:
    ws::Socket* _socket;
    ws::Repository _repo;
    uintptr_t _client_fd;
    ws::Kernel* _kernel;

    static ws::HeaderGenerator _header_generator;

    Response(const Response& other);
    Response& operator=(const Response& other);

  public:
    Response();
    ~Response();

    void init_response(ws::Socket* socket, client_value_type& client_data, uintptr_t client_fd);
    void set_kernel(Kernel* kernel);

    void generate_response(ws::Socket* socket, client_value_type& client_data, uintptr_t client_fd);

    std::string generate_directory_list() const throw();
  };
}
