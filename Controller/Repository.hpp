#pragma once

#include "Configure.hpp"
// #include "Socket.hpp"
#include "Request.hpp"

namespace ws {
  class Repository {
    public:
      typedef ws::Server::listen_type listen_type;
      typedef ws::Server::server_name_type server_name_type;
      typedef std::string location_dir_type;
      typedef ws::Server::location_map_type location_map_type;
      typedef ws::Location::limit_except_vec_type limit_except_vec_type;
      typedef ws::Location::return_type return_type;
      typedef ws::Location::cgi_type cgi_type;
      typedef ws::InnerOption::autoindex_type autoindex_type;
      typedef ws::InnerOption::root_type root_type;
      typedef ws::InnerOption::index_vec_type index_vec_type;
      typedef ws::InnerOption::client_max_body_size_type client_max_body_size_type;
      typedef ws::InnerOption::error_page_map_type error_page_map_type;

    private:
    /*server*/
      const listen_type* _listen;
      const server_name_type* _server_name;
      const location_dir_type* _location_dir;
    /*location*/
      const limit_except_vec_type* _limit_except_vec;
      const return_type* _return;
      const cgi_type* _cgi;
    /*option*/
      const autoindex_type* _autoindex;
      const root_type* _root;
      const index_vec_type* _index_vec;
      const client_max_body_size_type* _client_max_body_size;
      const error_page_map_type* _error_page_map;

    public:
      Repository();
      Repository(const Repository& cls);
      ~Repository();

      void operator()(const ws::Server* curr_server, const ws::Request& request);

    /*getter*/
      const listen_type& get_listen() const throw();
      const server_name_type& get_server_name() const throw();
      const location_dir_type& get_dir() const throw();
      const limit_except_vec_type& get_limit_except_vec() const throw();
      const return_type& get_return() const throw();
      const cgi_type& get_cgi() const throw();
      const autoindex_type& get_autoindex() const throw();
      const root_type& get_root() const throw();
      const index_vec_type& get_index_vec() const throw();
      const client_max_body_size_type& get_client_max_body_size() const throw();
      const error_page_map_type& get_error_page_map() const throw();

      void set_option(const ws::InnerOption& option);
  };
}
