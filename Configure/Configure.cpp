#include "Configure.hpp"

ws::Configure::Configure(const std::string& file, const std::string& curr_dir)
    : _parser(file, curr_dir), _server(_parser.parse()) {}

ws::Configure::~Configure() {}

const std::vector<ws::Server>& ws::Configure::get_server() const throw() {
  return _server;
}

ws::Configure::listen_type ws::Configure::get_host_list() const {
  listen_type ret;

  for (std::vector<ws::Server>::size_type i = 0; i < _server.size(); ++i) {
    listen_type curr = _server[i].get_listen();

    for (listen_type::size_type j = 0; j < curr.size(); ++j)
      ret.push_back(curr[j]);
  }

  return ret;
}
