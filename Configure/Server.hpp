#pragma once

#include "Location.hpp"

namespace ws {
  class Server {
  public:
    typedef std::vector<std::pair<u_int32_t, u_int16_t> > listen_type;
    typedef listen_type::value_type listen_value_type;
    typedef std::vector<std::string> server_name_type;
    typedef server_name_type::value_type server_name_value_type;
    typedef std::map<std::string, ws::Location> location_type;
    typedef location_type::value_type location_value_type;
    typedef ws::InnerOption::autoindex_type autoindex_type;
    typedef ws::InnerOption::root_type root_type;
    typedef ws::InnerOption::index_type index_type;
    typedef ws::InnerOption::index_value_type index_value_type;
    typedef ws::InnerOption::client_max_body_size_type client_max_body_size_type;
    typedef ws::InnerOption::error_page_type error_page_type;
    typedef ws::InnerOption::error_page_value_type error_page_value_type;

  private:
    listen_type _listen;
    server_name_type _server_name;
    location_type _location;
    ws::InnerOption _option;

  public:
    Server();
    ~Server();

    const listen_type& get_listen() const throw();
    const server_name_type& get_server_name() const throw();
    const location_type& get_location() const throw();
    const ws::InnerOption& get_option() const throw();
    
    const autoindex_type& get_autoindex() const throw();
    const root_type& get_root() const throw();
    const index_type& get_index() const throw();
    const client_max_body_size_type& get_client_max_body_size() const throw();
    const error_page_type& get_error_page() const throw();

    void set_listen(const listen_value_type& value);
    void set_server_name(const server_name_value_type& value);

    void set_location(const location_type& value);
    void set_option(const ws::InnerOption& value);

    void set_autoindex(const autoindex_type& value);
    void set_root(const root_type& value);
    void set_index(const index_value_type& value);
    void set_client_max_body_size(const client_max_body_size_type& value);
    void set_error_page(const error_page_value_type& value);
  };
}
