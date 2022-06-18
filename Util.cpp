#include "Util.hpp"

#include <fstream>
#include <stdexcept>
#include <sstream>

#include <cstdlib>

#include <unistd.h>

static const std::string hash = "Seunkim123rootjaham456projectyeju789dir";

char* ws::Util::_curr_dir = getcwd(NULL, 0);

ws::Util::Util() {}

ws::Util::~Util() {
  std::free(_curr_dir);
}

void ws::Util::check_executed_dir() {
  std::fstream identifier;
  identifier.open(Util::get_dir() + "/project_id", std::fstream::in);

  if (identifier.fail() || identifier.bad())
    throw std::runtime_error("Error: webserv must executed in project root path");

  std::stringstream buffer;
  buffer << identifier.rdbuf();
  identifier.close();

  std::string line;
  std::getline(buffer, line);

  if (line != hash || !buffer.eof())
    throw std::runtime_error("Error: webserv must executed in project root path");
}

std::string ws::Util::get_dir() throw() {
  return _curr_dir;
}
