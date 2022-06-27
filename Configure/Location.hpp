#pragma once

#include "InnerOption.hpp"

namespace ws {
  class Location {
  public:
    typedef std::map<std::string, int> limit_except_type;
    typedef std::pair<int, std::string> return_type;
    typedef std::string cgi_type;
  
  private:
    limit_except_type _limit_except;
    return_type _return;
    cgi_type _cgi;
    ws::InnerOption _option;

  public:
    Location();
    ~Location();

    void init_limit_except();

    limit_except_type get_limit_except() const throw();
    return_type get_return_type() const throw();
    cgi_type get_cgi() const throw();

    void set_limit_except(const std::string& method, bool value);
    void set_return_type(const return_type& value);
    void set_cgi(const cgi_type& value);
    void set_option(const ws::InnerOption& value);
  };
}  // namespace ws
