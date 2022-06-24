#pragma once

#include <map>
#include <string>
#include <vector>

#define kCLIENT_MAX_BODY_SIZE_LIMIT (ULONG_MAX - 1)
#define kCLIENT_MAX_BODY_SIZE_UNSET ULONG_MAX
#define kAUTOINDEX_UNSET -1

namespace ws {
  class InnerOption {
  public:
    typedef int autoindex_type;
    typedef std::string root_type;
    typedef std::vector<std::string> index_type;
    typedef index_type::value_type index_value_type;
    typedef unsigned long client_max_body_size_type;
    typedef std::map<int, std::string> error_page_type;
    typedef error_page_type::value_type error_page_value_type;

  private:
    autoindex_type _autoindex;
    root_type _root;
    index_type _index;
    client_max_body_size_type _client_max_body_size;
    error_page_type _error_page;

  public:
    InnerOption();
    ~InnerOption();

    const autoindex_type& get_autoindex() const throw();
    const root_type& get_root() const throw();
    const index_type& get_index() const throw();
    const client_max_body_size_type& get_client_max_body_size() const throw();
    const error_page_type& get_error_page() const throw();

    void set_autoindex(const autoindex_type& value) throw();
    void set_root(const root_type& value) throw();
    void set_index(const index_value_type& value) throw();
    void set_client_max_body_size(const client_max_body_size_type& value) throw();
    void set_error_page(const error_page_value_type& value) throw();
  };
}
