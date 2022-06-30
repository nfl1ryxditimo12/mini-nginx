#include <iostream>

#include "ConfParser.hpp"
#include "Util.hpp"
#include "Socket.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./webserv config/example.conf" << std::endl;
    return 1;
  }

  ws::Socket socket(std::stoi(argv[1]));

  ws::Configure conf;

  {
    ws::ConfParser config_parser(argv[1], ws::get_curr_dir());
    config_parser.parse(conf);
  }

  conf.print_configure();
}
