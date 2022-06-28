#include "Configure.hpp"

ws::Configure::Configure(const std::string& file, const std::string& curr_dir)
    : _parser(file, curr_dir), _server_vec(_parser.parse()) {}

ws::Configure::~Configure() {}

const ws::Configure::server_vec_type& ws::Configure::get_server_vec() const throw() {
  return _server_vec;
}

ws::Configure::listen_vec_type ws::Configure::get_host_list() const {
  listen_vec_type ret;

  for (std::vector<ws::Server>::size_type i = 0; i < _server_vec.size(); ++i) {
    listen_vec_type curr = _server_vec[i].get_listen_vec();

    for (listen_vec_type::size_type j = 0; j < curr.size(); ++j)
      ret.push_back(curr[j]);
  }

  return ret;
}
