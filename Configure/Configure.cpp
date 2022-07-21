#include "Configure.hpp"

#include <iostream>

ws::Configure::Configure() throw() {}

ws::Configure::~Configure() {}

ws::Configure &ws::Configure::operator=(const ws::Configure &other) {
  _server_vec = other._server_vec;
  _server_finder = other._server_finder;
  return *this;
}

const ws::Configure::server_vec_type& ws::Configure::get_server_vec() const throw() {
  return _server_vec;
}

void ws::Configure::set_server_vec(const server_vec_type& value) {
  _server_vec = value;
}

void ws::Configure::set_server_finder(const server_finder_type& value) {
  _server_finder = value;
}

ws::Configure::listen_vec_type ws::Configure::get_host_list() const {
  listen_vec_type ret;
  std::set<listen_type> duplicate_checker;

  for (server_vec_type::const_iterator it = _server_vec.begin(); it != _server_vec.end(); ++it) {
    const listen_vec_type& curr = it->get_listen_vec();

    for (listen_vec_type::const_iterator it_ = curr.begin(); it_ != curr.end(); ++it_) {
      if ((duplicate_checker.insert(*it_)).second)
        ret.push_back(*it_);
    }
  }

  return ret;
}

const ws::Server& ws::Configure::find_server(
  const listen_type& listen, const server_name_type& server_name
) const throw() {

  server_finder_type::key_type key(listen, server_name);

  server_finder_type::const_iterator result = _server_finder.find(key);

  if (result == _server_finder.end()) {
    key.second = "_";
    return _server_finder.find(key)->second;
  }

  return result->second;
}

void ws::Configure::print_server(const ws::Server& server) const throw() {
  std::cout << "\n--listen list--\n";
  const listen_vec_type& listen_vec = server.get_listen_vec();
  for (listen_vec_type::const_iterator it = listen_vec.begin(); it != listen_vec.end(); ++it)
    std::cout << "host: " << it->first << ", port: " << it->second << "\n";

  const server_name_vec_type& server_name_vec = server.get_server_name_vec();
  for (server_name_vec_type::const_iterator it = server_name_vec.begin(); it != server_name_vec.end(); ++it)
    std::cout << "server name: " << *it << "\n";

  std::cout << "\n----server option end----\n";

  this->print_option(server.get_option());

  std::cout << "\n----server inner option end----\n";

  this->print_location_map(server.get_location_map());

  std::cout << "\n----server end----\n";
}

void ws::Configure::print_location_map(const location_map_type& location_map) const throw() {
  std::cout << "\n--location_blocks--\n";
  for (location_map_type::const_iterator it = location_map.begin(); it != location_map.end(); ++it) {
    std::cout << "\n-block dir " << it->first << " -\n";
    this->print_location(it->second);
  }

  std::cout << "\n----location blocks end----\n";
}

void ws::Configure::print_location(const ws::Location& location) const throw() {
  const limit_except_vec_type& limit_except_vec = location.get_limit_except_vec();
  std::cout << "limit exception: ";
  for (limit_except_vec_type::const_iterator it = limit_except_vec.begin(); it != limit_except_vec.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << "\n";

  std::cout << "return status code: " << location.get_return().first << " value: " << location.get_return().second << "\n";

  const cgi_map_type& cgi_set = location.get_cgi_map();
  std::cout << "cgi: " << "\n";
  for (cgi_map_type::const_iterator it = cgi_set.begin(); it != cgi_set.end(); ++it) {
    std::cout << "  " << it->first << ", " << it->second << std::endl;
  }

  std::cout << "\n----location option end----\n";

  this->print_option(location.get_option());

  std::cout << "\n----location inner option end----\n";

  std::cout << "\n----location end----\n";
}

void ws::Configure::print_option(const ws::InnerOption& option) const throw() {
  std::cout << "autoindex: " << option.get_autoindex() << "\n";
  std::cout << "root: " << option.get_root() << "\n";

  const index_set_type& index_set = option.get_index_set();
  std::cout << "index: ";
  for (index_set_type::const_iterator it = index_set.begin(); it != index_set.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << "\n";

  std::cout << "client_max_body_size: " << option.get_client_max_body_size() << "\n";

  const error_page_map_type& error_page_map = option.get_error_page_map();
  for (error_page_map_type::const_iterator it = error_page_map.begin(); it != error_page_map.end(); ++it)
    std::cout << "error status code: " << it->first << ", value: " << it->second << "\n";
}

void ws::Configure::print_configure() const throw() {
  std::size_t cnt = 1;

  for (server_vec_type::const_iterator it = _server_vec.begin(); it != _server_vec.end(); ++it, ++cnt) {
    std::cout << "===========server block " << cnt << " ===========\n";
    this->print_server(*it);
  }

  std::cout << "print configure end" << std::endl;
}
