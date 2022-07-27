#pragma once

#include "Server.hpp"

namespace ws {
  class Configure {
  public:
    typedef std::vector<ws::Server> server_vec_type;

    typedef ws::Server::listen_type listen_type;
    typedef ws::Server::listen_vec_type listen_vec_type;
    typedef ws::Server::server_name_type server_name_type;
    typedef ws::Server::server_name_vec_type server_name_vec_type;

    typedef ws::Server::location_map_type location_map_type;
    typedef ws::Location::limit_except_vec_type limit_except_vec_type;
    typedef ws::Location::cgi_map_type cgi_map_type;
    typedef ws::InnerOption::index_set_type index_set_type;
    typedef ws::InnerOption::error_page_type error_page_type;
    typedef ws::InnerOption::error_page_map_type error_page_map_type;

    typedef std::map<std::pair<listen_type, server_name_type>, const Server&> server_finder_type;

  private:
    server_vec_type _server_vec;
    server_finder_type _server_finder;

    Configure(const Configure& other);

  public:
    Configure() throw();
    Configure& operator=(const Configure& other);
    ~Configure();

    const server_vec_type& get_server_vec() const throw();

    void set_server_vec(const server_vec_type& value);
    void set_server_finder(const server_finder_type& value);

    // returns server vector for binding socket
    listen_vec_type get_host_list() const;
    // find server block with listen, server_name for set ws::Repository
    const Server& find_server(const listen_type& listen, const server_name_type& server_name) const throw();

    void print_server(const ws::Server& server) const throw();
    void print_location_map(const location_map_type& location_map) const throw();
    void print_location(const ws::Location& location) const throw();
    void print_option(const ws::InnerOption& option) const throw();
    void print_configure() const throw();
  };
}
