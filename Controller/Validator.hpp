#pragma once

#include "Socket.hpp"
#include "Util.hpp"

namespace ws {
  class Validator {
  public:
    // typedef ws::Request::header_type header_type_;
    // typedef header_type_::iterator	header_iterator;
    typedef ws::Socket::client_value_type client_value_type;

    typedef void (Validator::*func)(const std::string&);
    typedef std::map<std::string, func> validate_func;

    typedef ws::Request::header_type  header_type;

  private:
    // const ws::Request _request;
    // const ws::Repository _repository;

    validate_func _validate_func;

    void check_method();
    void check_uri();
    void check_version();

    Validator(const Validator& cls);
    Validator& operator=(const Validator& cls);
    
  public:
    Validator();
    ~Validator();

    void operator()(const client_value_type* client_data);

    void check_content_length(const std::string&);
    void check_connection(const std::string&);
    void check_content_type(const std::string&);
    void check_transger_encoding(const std::string&);
    void check_host(const std::string&);
    void check_request_header();

  };
}
