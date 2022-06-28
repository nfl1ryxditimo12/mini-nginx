#pragma once

#include "InnerOption.hpp"

namespace ws {
  class Location {
  public:
    typedef std::pair<std::string, int> limit_except_type;
    typedef std::map<limit_except_type::first_type, limit_except_type::second_type> limit_except_map_type;
    typedef std::pair<int, std::string> return_type;
    typedef std::string cgi_type;
    typedef ws::InnerOption::autoindex_type autoindex_type;
    typedef ws::InnerOption::root_type root_type;
    typedef ws::InnerOption::index_vec_type index_vec_type;
    typedef ws::InnerOption::index_type index_type;
    typedef ws::InnerOption::client_max_body_size_type client_max_body_size_type;
    typedef ws::InnerOption::error_page_map_type error_page_map_type;
    typedef ws::InnerOption::error_page_type error_page_type;
  
  private:
    limit_except_map_type _limit_except_map;
    return_type _return;
    cgi_type _cgi;
    ws::InnerOption _option;

  public:
    Location();
    ~Location();

    void init_limit_except();

    const limit_except_map_type& get_limit_except_map() const throw();
    const return_type& get_return() const throw();
    const cgi_type& get_cgi() const throw();
    const ws::InnerOption& get_option() const throw();

    const autoindex_type& get_autoindex() const throw();
    const root_type& get_root() const throw();
    const index_vec_type& get_index_vec() const throw();
    const client_max_body_size_type& get_client_max_body_size() const throw();
    const error_page_map_type& get_error_page_map() const throw();

    void set_limit_except(const std::string& method, bool value);
    void set_return(const return_type& value);
    void set_cgi(const cgi_type& value);
    void set_option(const ws::InnerOption& value);

    void set_autoindex(const autoindex_type& value);
    void set_root(const root_type& value);
    void set_index(const index_type& value);
    void set_client_max_body_size(const client_max_body_size_type& value);
  };
}  // namespace ws
