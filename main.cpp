#include <iostream>

#include "ConfParser.hpp"
#include "Util.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./webserv config/example.conf" << std::endl;
    return 1;
  }

  ws::check_executed_dir();

  ws::Configure conf;

  {
    ws::ConfParser config_parser(argv[1], ws::get_curr_dir());
    config_parser.parse(conf);
  }
}
