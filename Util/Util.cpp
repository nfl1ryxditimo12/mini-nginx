#include "Util.hpp"

#include <fstream>
#include <stdexcept>
#include <sstream>

#include <cstdlib>

#include <unistd.h>

// hash for checking executed directory
const std::string ws::Util::_hash = "Seunkim123rootjaham456projectyeju789dir";

// current executed directory
// initialized by calling get_root_dir()
const std::string ws::Util::_root_dir = ws::Util::set_curr_dir();

// param: void
// return: std::string root dir
// exception: throws when allocation failed, or webserv was not executed at project root
// description: initialize curr_dir
std::string ws::Util::set_curr_dir() {
  char* c_curr_dir = getcwd(NULL, 0);

  std::string ret(c_curr_dir);
  free(c_curr_dir);

  Util::check_executed_dir();

  return ret;
}

// param: void
// return: (std::string) current executed directory
// exception: none
// description: returns curr_dir
const std::string& ws::Util::get_root_dir() {
  return _root_dir;
}

// param: void
// return: void
// exception: throws when current working directory is not project root
// description: check executed directory is root
void ws::Util::check_executed_dir() {
  std::fstream identifier;
  identifier.open(Util::get_root_dir() + "/project_id", std::fstream::in);

  if (identifier.fail() || identifier.bad())
    throw std::runtime_error("Error: webserv must executed in project root path");

  std::stringstream buffer;
  buffer << identifier.rdbuf();
  identifier.close();

  std::string line;
  std::getline(buffer, line);

  if (line != _hash || !buffer.eof())
    throw std::runtime_error("Error: webserv must executed in project root path");
}

// param: string to convert, min, max limit of converted result
// return: unsigned long value, ULONG_MAX if failed
// exception none

unsigned long ws::Util::stoul(const std::string& str, unsigned long max, unsigned long min) throw() {
  unsigned long ret = 0;

  for (std::string::size_type i = 0; i < str.length(); i++) {
    if (!std::isdigit(str[i]))
      return std::numeric_limits<unsigned long>::max();
    if (ret > max / 10)
      return std::numeric_limits<unsigned long>::max();
    ret *= 10;

    if (ret > max - (str[i] - '0'))
      return std::numeric_limits<unsigned long>::max();
    ret += str[i] - '0';
  }

  return ret - ((ret - std::numeric_limits<unsigned long>::max()) * (ret < min));
}

unsigned long ws::Util::hextoul(const std::string& str, unsigned long max, unsigned long min) throw() {
  std::string hex = "0123456789ABCDEF";
  unsigned long ret = 0;

  for (std::string::size_type i = 0; i < str.length(); i++) {
    int chartohex = hex.find(static_cast<char>(std::toupper(str[i]))) + 1;

    if (!std::isxdigit(str[i]))
      return std::numeric_limits<unsigned long>::max();
    if (ret > max / 16)
      return std::numeric_limits<unsigned long>::max();
    ret *= 16;

    if (ret > max - chartohex)
      return std::numeric_limits<unsigned long>::max();
    ret += chartohex;
  }

  return ret - ((ret - std::numeric_limits<unsigned long>::max()) * (ret < min));
}

std::string ws::Util::ultos(unsigned long n) throw() {
  std::stringstream buffer;

  buffer << n;
  return std::string(buffer.str());
}

bool ws::Util::is_valid_server_name(const std::string& str) {
  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
    if (!std::isalnum(*it) && (*it != '-'))
      return false;
  }

  return true;
}

std::string ws::Util::parse_relative_path(const std::string &str) {
  return str;
}
