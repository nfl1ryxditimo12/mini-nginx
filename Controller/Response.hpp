#pragma once

#include "Socket.hpp"
#include "HeaderGenerator.hpp"

namespace ws {
  class Response {
  public:
    typedef ws::Socket::client_value_type client_value_type;

  private:
    ws::Repository* _repo;
    uintptr_t _client_fd;
    ws::Kernel* _kernel;

    static ws::HeaderGenerator _header_generator;

    Response(const Response& other);
    Response& operator=(const Response& other);

  public:
    Response() throw();
    ~Response();

    void set_data(client_value_type& client_data, uintptr_t client_fd);
    void set_kernel(Kernel* kernel);

    void process(client_value_type& client_data, uintptr_t client_fd);
    void generate(client_value_type& client_data, uintptr_t client_fd);
  };
}
