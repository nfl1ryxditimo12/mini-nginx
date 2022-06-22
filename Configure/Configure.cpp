#include "Configure.hpp"

ws::Configure::Configure(const std::string& file, const std::string& curr_dir)
    : _parser(file), _server(_parser.parse(curr_dir)) {}

ws::Configure::~Configure() {}

const std::vector<ws::Server>& ws::Configure::get_server() const throw() {
  return _server;
}
