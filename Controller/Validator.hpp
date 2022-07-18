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

    typedef void (Validator::*check_func)(client_value_type&);
    typedef std::vector<check_func> check_func_vec;

    typedef ws::Request::header_type header_type;

    typedef ws::Location::limit_except_vec_type limit_except_vec_type;

    typedef ws::Socket::session_map_type session_map_type;

  private:
    // const ws::Request _request;
    // const ws::Repository _repository;

    validate_func _validate_func;
    check_func_vec _check_func_vec;

    const session_map_type* _session;

    Validator(const Validator& cls);
    Validator& operator=(const Validator& cls);
    
  public:
    Validator() throw();
    ~Validator();

    void operator()(const session_map_type& session, client_value_type& client_data);

    void check_method(client_value_type& client_data);
    void check_uri(client_value_type& client_data);
    void check_version(client_value_type& client_data);

    void check_host(client_value_type& client_data);
    void check_connection(client_value_type& client_data);
    void check_content_length(client_value_type& client_data);
    void check_transfer_encoding(client_value_type& client_data);
    void check_session_id(client_value_type& client_data);
//    void check_name(client_value_type& client_data);
    void check_secret_key(client_value_type& client_data);

    void check_request_header(client_value_type& client_data);
  };
}
