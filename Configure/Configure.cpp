#include "Configure.hpp"

ws::Configure::Configure(const std::string& file, const std::string& curr_dir)
    : _parser(file, curr_dir), _server(_parser.parse()) {}

ws::Configure::~Configure() {}

const std::vector<ws::Server>& ws::Configure::get_server() const throw() {
  return _server;
}
