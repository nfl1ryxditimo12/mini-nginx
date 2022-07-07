#pragma once

#include "Socket.hpp"
#include "ResponseHeader.hpp"

namespace ws {
  class Response {
  public:
    typedef ws::Socket::client_value_type client_value_type;

    typedef ws::Server::error_page_map_type error_page_map_type;

  private:
    const ws::Request& _request;
    const ws::Repository& _repo;
    unsigned int _status;
    ws::Kernel _kernel;
    ResponseHeader _header;
    std::string _data;

    bool is_error_status(unsigned int stat) const throw();
    void get_error_response();

    std::string process_get(client_value_type& client);
    std::string process_post(client_value_type& client);
    std::string process_delete(client_value_type& client);

    void generate_response_header(std::string::size_type content_length);
    std::string generate_general_header();
    std::string generate_date();

    void read_file(const char* file);

    Response();
    Response(const Response& other);
    Response& operator=(const Response& other);

  public:
    explicit Response(const client_value_type* const client_data);
    ~Response();

    void generate_response();

    const std::string& get_data() const throw();
  };
}
