#pragma once

#include "InnerOption.hpp"

namespace ws {
  class Location {
  public:
    typedef std::pair<std::string, int> limit_except_type;
    typedef std::map<limit_except_type::first_type, limit_except_type::second_type> limit_except_map_type;
    typedef std::pair<int, std::string> return_type;
    typedef std::string cgi_type;
  
  private:
    limit_except_map_type _limit_except_map;
    return_type _return;
    cgi_type _cgi;
    ws::InnerOption _option;

  public:
    Location();
    ~Location();

    void init_limit_except();

    const limit_except_map_type& get_limit_except() const throw();
    const return_type& get_return_type() const throw();
    const cgi_type& get_cgi() const throw();
    const ws::InnerOption& get_option() const throw();

    void set_limit_except(const std::string& method, bool value);
    void set_return_type(const return_type& value);
    void set_cgi(const cgi_type& value);
    void set_option(const ws::InnerOption& value);
  };
}  // namespace ws
