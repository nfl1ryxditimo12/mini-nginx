#pragma once

#include <map>
#include <string>
#include <vector>

#define kCLIENT_MAX_BODY_SIZE_LIMIT (ULONG_MAX - 1)
#define kCLIENT_MAX_BODY_SIZE_UNSET -1UL
#define kAUTOINDEX_UNSET -1

namespace ws {
  class InnerOption {
  public:
    typedef int autoindex_type;
    typedef std::string root_type;
    typedef std::string index_type;
    typedef std::vector<index_type> index_vec_type;
    typedef unsigned long client_max_body_size_type;
    typedef std::pair<unsigned int, std::string> error_page_type;
    typedef std::map<error_page_type::first_type, error_page_type::second_type> error_page_map_type;

  private:
    autoindex_type _autoindex;
    root_type _root;
    index_vec_type _index_vec;
    client_max_body_size_type _client_max_body_size;
    error_page_map_type _error_page_map;

  public:
    InnerOption();
    ~InnerOption();

    const autoindex_type& get_autoindex() const throw();
    const root_type& get_root() const throw();
    const index_vec_type& get_index_vec() const throw();
    const client_max_body_size_type& get_client_max_body_size() const throw();
    const error_page_map_type& get_error_page_map() const throw();

    void set_autoindex(const autoindex_type& value);
    void set_root(const root_type& value);
    void add_index(const index_type& value);
    void set_client_max_body_size(const client_max_body_size_type& value);
    void add_error_page(const error_page_type& value);
  };
}
