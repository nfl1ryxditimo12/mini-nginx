#include "Util.hpp"

#include <fstream>
#include <stdexcept>
#include <sstream>

#include <cstdlib>

#include <unistd.h>

ws::Util::Util() {}
ws::Util::~Util() {}

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

unsigned long ws::Util::stoul(
  const std::string& str, unsigned long max, unsigned long min, const std::string& base_str
) throw() {
  unsigned long ret = 0;

  for (
    std::string::size_type i = 0, pos = base_str.find(static_cast<char>(std::toupper(str[i])));
    i < str.length();
    i++, pos = base_str.find(static_cast<char>(std::toupper(str[i])))
  ) {
    if (pos == std::string::npos)
      return std::numeric_limits<unsigned long>::max();

    if (ret > max / base_str.length())
      return std::numeric_limits<unsigned long>::max();
    ret *= base_str.length();

    if (ret > max - pos)
      return std::numeric_limits<unsigned long>::max();
    ret += pos;
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
  std::string ret;

  for (
    std::string::size_type pos1 = 0, pos2 = str.find('/', pos1 + 1);
    pos1 != std::string::npos;
    pos1 = pos2, pos2 = str.find('/', pos1 + 1)
  ) {
    if (!(str.compare(pos1 + 1, 3, "../") && str.compare(pos1 + 1, str.length() - pos1 - 1, ".."))) {
      std::string::size_type erase_point = ret.find_last_of('/');

      if (erase_point != std::string::npos)
        ret.erase(erase_point);

      continue;
    }

    if (!(str.compare(pos1 + 1, 2, "./") && str.compare(pos1 + 1, str.length() - pos1 - 1, ".")) || str[pos1 + 1] == '/')
      continue;

    if (pos2 == std::string::npos) {
      ret.append(str, pos1, str.length() - pos1);
      break;
    }

    ret.append(str, pos1, pos2 - pos1);
  }

  if ((ret[ret.length() - 1] == '/') && (ret.length() != 1))
      ret.erase(ret.end() - 1);

  if (ret[0] != '/')
      ret = "/" + ret;

  return ret;
}

// returns true if lseek returns error. (means return is too large to contain in off_t)
bool ws::Util::is_eof(int fd) throw() {
  off_t curr = lseek(fd, 0, SEEK_CUR);
  off_t end = lseek(fd, 0, SEEK_END);
  lseek(fd, curr, SEEK_SET);

  return curr == end || curr < 0 || end < 0;
}

std::string ws::Util::format_number(size_t num) {
  std::string ret = "";

  for (int digit = ultos(num).length(); digit > 0;) {
    int div = static_cast<int>(std::pow(10, digit - 1));
    ret += ultos(static_cast<int>(num / div));
    --digit;
    if (digit % 3 == 0 && digit != 0)
      ret += ",";
    num = num % div;
  }
  return ret;
}

std::string ws::Util::mime_type(const std::string& extension) {
  if (extension == "jpeg")
    return "image/jpeg";
  else if (extension == "png")
    return "image/png";
  else if (extension == "jpg")
    return "image/jpg";
  else if (extension == "html")
    return "text/html";
  else if (extension == "out")
    return "application/octet-stream";
  else
    return "text";
}
#include <sys/types.h>
void ws::Util::print_accept_client(const std::string& request, u_int32_t index, int fd) {
  std::cout << CYN << request << NC << " client_idx: " << index << ", accepted fd: " << fd << std::endl;
}

void ws::Util::print_response_client(int status, int fd) {
  std::cout << GRN << "[Response]" << NC << " - status: " << status << ", client fd: " << fd << std::endl;
}

void ws::Util::print_disconnect_client(int fd, const std::string& cause) {
  std::cout << RED << "[Disconnect]" << NC << " - client fd: " << fd << YLW << " " + cause << NC << std::endl;
}