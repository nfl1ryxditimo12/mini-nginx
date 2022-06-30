#include "Configure.hpp"
#include "Socket.hpp"

namespace ws {
  class Repository {
    public:
      typedef ws::Server::listen_vec_type listen_vec_type;
      typedef ws::Server::server_name_vec_type server_name_vec_type;
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
      listen_vec_type _listen_vec;
      server_name_vec_type _server_name_vec;
    /*location*/
      limit_except_vec_type _limit_except_vec;
      return_type _return;
      cgi_type _cgi;
    /*option*/
      autoindex_type _autoindex;
      root_type _root;
      index_vec_type _index_vec;
      client_max_body_size_type _client_max_body_size;
      error_page_map_type _error_page_map;

    public:
      Repository();
      ~Repository();

    /*getter*/
      const listen_vec_type& get_listen_vec() const throw();
      const server_name_vec_type& get_server_name_vec() const throw();
      const limit_except_vec_type& get_limit_except_vec() const throw();
      const return_type& get_return() const throw();
      const cgi_type& get_cgi() const throw();
      const autoindex_type& get_autoindex() const throw();
      const root_type& get_root() const throw();
      const index_vec_type& get_index_vec() const throw();
      const client_max_body_size_type& get_client_max_body_size() const throw();
      const error_page_map_type& get_error_page_map() const throw();

    /*setter*/
      void set_listen_vec(const listen_vec_type& value);
      void set_server_name_vec(const server_name_vec_type& value);
      void set_limit_except_vec(const limit_except_vec_type& value);
      void set_return(const return_type& value);
      void set_cgi(const cgi_type& value);
      void set_autoindex(const autoindex_type& value);
      void set_root(const root_type& value);
      void set_index_vec(const index_vec_type& value);
      void client_max_body_size(const client_max_body_size_type& value);
      void error_page_map(const error_page_map_type& value);

      void get_repository(const ws::Configure* _conf, const ws::Request* _request);
      void set_repository(const ws::Server curr_server, const ws::Request* _requset);
      void set_option(const ws::InnerOption& option);
  };
}
