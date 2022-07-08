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

    void set_data(ws::Socket* socket, client_value_type& client_data, uintptr_t client_fd);
    void set_kernel(Kernel* kernel);

    void process(ws::Socket* socket, client_value_type& client_data, uintptr_t client_fd);
    void generate(ws::Socket* socket, client_value_type& client_data, uintptr_t client_fd);

    std::string generate_directory_list() const;
    std::string generate_directory_list_body() const;
  };
}
