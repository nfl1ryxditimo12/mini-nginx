#include "ConfParser.hpp"

#include <arpa/inet.h>
#include <fstream>
#include <stdexcept>

#include <cctype>

#include "Util.hpp"

ws::ConfParser::ConfParser() {}

ws::ConfParser::~ConfParser() {}

std::string ws::ConfParser::open_file(const std::string& file) const {
  std::ifstream input;
  if (file.rfind(".conf") != file.length() - 5)
    throw std::invalid_argument("Configure: wrong file extension");

  input.open(file, std::ifstream::in);
  if (input.fail() || input.bad())
    throw std::invalid_argument("Configure: fail to open file");

  std::stringstream buffer;
  buffer << input.rdbuf();
  input.close();

  std::string ret(buffer.str());
  return ret;
}

void  ws::ConfParser::check_block_header(const std::string& line, const std::string& block_name) const {
  std::string::size_type pos = ws::skip_whitespace(line);

  if (line.find(block_name) != pos)
    throw std::invalid_argument("Configure: wrong " + block_name + " header");

  pos = ws::skip_whitespace(line, pos + 6);
  if (line.find_first_of('{') != pos)
    throw std::invalid_argument("Configure: wrong " + block_name + " header");

  pos = ws::skip_whitespace(line, pos + 1);
  if (pos != line.npos)
    throw std::invalid_argument("Configure: wrong " + block_name + " header");  
}
#include <iostream> //TOdO
ws::Server ws::ConfParser::parse_server(std::stringstream& buffer, const std::string& curr_dir) const {
  ws::Server ret;
(void) curr_dir;
  std::string line;
  std::string::size_type pos;

  server_parser_func_map server_parser_func;

  this->init_server_parser_func(server_parser_func);

  while (!buffer.eof()) {
    std::getline(buffer, line);
    if (!line.length())
      continue;

    pos = ws::skip_whitespace(line);
    std::string key = line.substr(pos, line.find_first_of(" \t", pos) - pos);
    (server_parser_func.find(key)->second)(ret, line, ws::skip_whitespace(line, pos + key.length()));
    exit(1);
    
    // if (!line.compare(pos, 6, "listen"))
    //   ret.set_listen(this->parse_listen(line, skip_whitespace(line, pos + 6)));
    // else if (!line.compare(pos, 11, "server_name"))
    //   ret.set_server_name(this->parse_server_name(line, skip_whitespace(line, pos + 11)));
    // else if (!line.compare(pos, 12, "limit_except"))
    //   ret.set_limit_except(this->parse_limit_except(line), skip_whitespace(line, pos + 12));
    // else if (!line.compare(pos, 6, "return"))
    //   ret.set_return(this->parse_return(line), skip_whitespace(line, pos + 6));
    // else if (!line.compare(pos, 20, "client_max_body_size"))
    //   ret.set_client_max_body_size(this->parse_client_max_body_size(line), skip_whitespace(line, pos + 20));
    // else if (!line.compare(pos, 14, "directory_list"))
    //   ret.set_directory_list(this->parse_directory_list(line), skip_whitespace(line, pos + 14));
    // else if (!line.compare(pos, 4, "root"))
    //   ret.set_root(this->parse_root(line), skip_whitespace(line, pos + 4));
    // else if (!line.compare(pos, 5, "index"))
    //   ret.set_index(this->parse_index(line), skip_whitespace(line, pos + 5));
    // else if (!line.compare(pos, 10, "error_page"))
    //   ret.set_error_page(this->parse_error_page(line), skip_whitespace(line, pos + 10));
    // else
    //   throw std::invalid_argument("Configure: wrong configure key");
  }
  return ret;
}

void ws::ConfParser::init_server_parser_func(server_parser_func_map& server_parser_func) const {
  server_parser_func.insert(server_parser_func_map::value_type("listen", ConfParser::parse_listen));
}

void ws::ConfParser::parse_listen(ws::Server& server, const std::string& line, std::string::size_type pos) {
  std::pair<u_int32_t, int> value;

  if ((line.find(':', pos) == line.npos) && (line.find('.', pos) == line.npos))
    value.first = INADDR_ANY;
  else
    value.first = ws::ConfParser::parse_listen_host_ip(line, pos);

  value.second = ws::ConfParser::parse_listen_port(line, pos);

  pos = skip_whitespace(line, pos);
  if (line[pos])
    throw std::invalid_argument("Configure: wrong host ip address: invalid format");

  server.set_listen(value);
}

// localhost: 127.0.0.1
static const int kLOCALHOST = 2130706433;

u_int32_t ws::ConfParser::parse_listen_host_ip(const std::string& line, std::string::size_type& pos) {
  if (!line.compare(pos, 9, "localhost")) {
    pos += 9;
    return kLOCALHOST;
  }

  if (line[pos] == ':')
    throw std::invalid_argument("Configure: wrong host ip address: no host for port");

  u_int32_t ret = 0;
  u_int32_t temp = 0;
  std::string::size_type cnt = 0;

  for (; ; ++pos) {
    if (((line[pos] == '.') || ((line[pos] == ':') || (line[pos] == ';'))) && cnt) {
      ret <<= 8;
      ret |= temp;
      temp = 0;
      continue;
    }

    if ((line[pos] == ':') || (line[pos] == ';'))
      break;

    if (!std::isdigit(line[pos]))
      throw std::invalid_argument("Configure: wrong host ip address: invalid format");

    temp *= 10;
    temp += line[pos] - '0';
    ++cnt;

    if (temp > UINT8_MAX)
      throw std::invalid_argument("Configure: wrong host ip address: too large number for ip address");
  }

std::cout << ret<< std::endl;
  return htonl(ret);
}

u_int16_t ws::ConfParser::parse_listen_port(const std::string& line, std::string::size_type& pos) {
  if (line[pos] == ';')
    return htons(80);

  if (line[pos] == ':')
    ++pos;

  u_int32_t ret = 0;

  for (; line[pos] != ';'; ++pos) {
    if (!std::isdigit(line[pos]))
      throw std::invalid_argument("Configure: wrong port: non numeric");

    ret *= 10;
    ret += line[pos] - '0';
    if (ret > UINT16_MAX)
      throw std::invalid_argument("Configure: wrong port: too large number");
  }

  ++pos;
  return htons(static_cast<u_int16_t>(ret));
}

std::vector<ws::Server> ws::ConfParser::parse(const std::string& file, const std::string& curr_dir) const {
  std::stringstream buffer(this->open_file(file));

  std::string line;
  std::vector<ws::Server> ret;

  while (!buffer.eof()) {
    std::getline(buffer, line);
    if (!line.length())
      continue;

    this->check_block_header(line, "server");
    ret.push_back(this->parse_server(buffer, curr_dir));
  }
  return ret;
}
