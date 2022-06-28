#include "Configure.hpp"

ws::Configure::Configure() {}

ws::Configure::~Configure() {}

const ws::Configure::server_vec_type& ws::Configure::get_server_vec() const throw() {
  return _server_vec;
}

void ws::Configure::set_server_vec(const server_vec_type& value) {
  _server_vec = value;
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
