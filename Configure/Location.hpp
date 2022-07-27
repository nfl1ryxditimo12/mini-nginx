#pragma once

#include <vector>

#include "InnerOption.hpp"

namespace ws {
  class Location {
  public:
    typedef bool session_type;
    typedef std::string limit_except_type;
    typedef std::vector<limit_except_type> limit_except_vec_type;
    typedef std::pair<unsigned int, std::string> return_type;
    typedef std::pair<std::string, std::string> cgi_type;
    typedef std::map<cgi_type::first_type, cgi_type::second_type> cgi_map_type;
    typedef std::string block_name_type;
    typedef ws::InnerOption::autoindex_type autoindex_type;
    typedef ws::InnerOption::root_type root_type;
    typedef ws::InnerOption::index_set_type index_set_type;
    typedef ws::InnerOption::index_type index_type;
    typedef ws::InnerOption::client_max_body_size_type client_max_body_size_type;
    typedef ws::InnerOption::error_page_map_type error_page_map_type;
  
  private:
    session_type _session;
    limit_except_vec_type _limit_except_vec;
    return_type _return;
    cgi_map_type _cgi_map;
    ws::InnerOption _option;
    block_name_type _block_name;

    Location& operator=(const Location& other);

  public:
    Location();
    explicit Location(const Location& other);
    ~Location();

    const session_type& get_session() const throw();
    const limit_except_vec_type& get_limit_except_vec() const throw();
    const return_type& get_return() const throw();
    const cgi_map_type& get_cgi_map() const throw();
    const ws::InnerOption& get_option() const throw();
    const block_name_type& get_block_name() const throw();

    const autoindex_type& get_autoindex() const throw();
    const root_type& get_root() const throw();
    const index_set_type& get_index_set() const throw();
    const client_max_body_size_type& get_client_max_body_size() const throw();
    const error_page_map_type& get_error_page_map() const throw();

    void set_session(const session_type& value);
    void add_limit_except(const limit_except_type& value);
    void set_return(const return_type& value);
    void add_cgi(const cgi_type& value);
    void set_cgi_map(const cgi_map_type& value);
    void set_option(const ws::InnerOption& value);
    void set_block_name(const block_name_type& value);

    void set_autoindex(const autoindex_type& value);
    void set_root(const root_type& value);
    void add_index(const index_type& value);
    void set_index(const index_set_type& value);
    void set_client_max_body_size(const client_max_body_size_type& value);
  };
}
