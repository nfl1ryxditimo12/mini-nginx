#include "ConfParser.hpp"

#include <algorithm>
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

void ws::ConfParser::check_block_header(Token& token, std::stringstream& buffer, const std::string& block_name) const {
  if (token != block_name)
    throw std::invalid_argument("Configure: wrong " + block_name + " header");

  token << buffer;
  if (token != '{')
    throw std::invalid_argument("Configure: wrong " + block_name + " header");

  token << buffer;
  if (token != '\n')
    throw std::invalid_argument("Configure: wrong " + block_name + " header");  
}

ws::Server ws::ConfParser::parse_server(Token& token, std::stringstream& buffer, const std::string& curr_dir) const {
  ws::Server ret;
(void) curr_dir;
  std::string line;

  server_parser_func_map server_parser_func;

  this->init_server_parser_func(server_parser_func);

  while (!buffer.eof()) {
    token << buffer;
    if (token == "\n")
      continue;

    server_parser_func_map::iterator iter;

    iter = server_parser_func.find(token.get_token());
    if (iter == server_parser_func.end())
      throw std::invalid_argument("Configure: wrong field key");
    iter->second(ret, token, buffer);
    token << buffer;
    if (token != "\n")
      throw std::invalid_argument("Configure: wrong field argument number");


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
  server_parser_func.insert(server_parser_func_map::value_type("server_name", ConfParser::parse_server_name));
  server_parser_func.insert(
    server_parser_func_map::value_type("parse_client_max_body_size", ConfParser::parse_client_max_body_size)
  );
}

// localhost: 127.0.0.1
static const int kLOCALHOST = 2130706433;

void ws::ConfParser::parse_listen(ws::Server& server, ws::Token& token, std::stringstream& buffer) {
  listen_value_type value;

  token << buffer;

  if (token[token.length() - 1] != ';')
    throw std::invalid_argument("Configure: listen: `;' should appear at eol");

  token.erase(token.length() - 1, 1);

  if (token.find(":") != token.npos) {
    ConfParser::parse_listen_host(token, ':', value);
    ConfParser::parse_listen_port(token, value);
    server.set_listen(value);
  } else if (token.find(".") != token.npos) {
    ConfParser::parse_listen_host(token, 0, value);
    value.second = htons(80);
    server.set_listen(value);
  } else {
    ConfParser::parse_listen_port(token, value);
    value.first = htonl(INADDR_ANY);
    server.set_listen(value);
  }
}

void ws::ConfParser::parse_listen_host(Token& token, char delim, listen_value_type& value) {
  ws::Token::size_type end;

  if (!token.compare(0, 9, "localhost")) {
    end = 9;
    value.first = htonl(kLOCALHOST);
  } else {
    end = std::min(token.find(":"), token.length());
    value.first = inet_addr(token.substr(0, end).c_str());
  }

  if ((token[end] != delim) || (value.first == INADDR_NONE))
    throw std::invalid_argument("Configure: listen: wrong host");
}

void ws::ConfParser::parse_listen_port(Token& token, listen_value_type& value) {
  if (!token.length())
    throw std::invalid_argument("Configure: listen: need port");

  u_int32_t port = 0;

  Token::size_type i = token.find(":");
  if (i == token.npos)
    i = 0;
  else
    ++i;

  for (; token[i]; ++i) {
    if (!std::isdigit(token[i]))
      throw std::invalid_argument("Configure: listen: non numeric value in port");

    port *= 10;
    port += token[i] - '0';
    if (port > UINT16_MAX)
      throw std::out_of_range("Configure: listen: port out of range");
  }

  value.second = htons(port);
}

void ws::ConfParser::parse_server_name(ws::Server& server, ws::Token& token, std::stringstream& buffer) {
  token << buffer;

  Token::size_type pos = token.find(";");
  if (pos != token.npos) {
    if (pos != token.length() - 1)
      throw std::invalid_argument("Configure: server_name: `;' should appear at eol");
    if (pos == 0)
      throw std::invalid_argument("Configure: server_name: `;' should appear at eol");
    server.set_server_name(token.substr(0, pos - 1));
  } else {
    if (!token.length())
      throw std::invalid_argument("Configure: server_name: invalid format");
    if (token == "\n")
      throw std::invalid_argument("Configure: server_name: invalid format");
    server.set_server_name(token.get_token());
  }
}

void ws::ConfParser::parse_client_max_body_size(ws::Server& server, ws::Token& token, std::stringstream& buffer) {
  token << buffer;

  if (token.find(";") == token.npos)
    throw std::invalid_argument("Configure: client_max_body_size: `;' should appear at eol");

  unsigned long size = 0;

  for (Token::size_type i = 0; token[i]; ++i) {
    if (!std::isdigit(token[i]))
      throw std::invalid_argument("Congfigure: client_max_body_size: non numeric value");

    if (size >= ULONG_MAX / 10)
      throw std::out_of_range("Configure: client_max_body_size: too large value");
    size *= 10;
    if (size >= ULLONG_MAX - (token[i] - '0'))
      throw std::out_of_range("Configure: client_max_body_size: too large value");
    size += token[i] - '0';
  }

  server.get_option().set_client_max_body_size(size);
}

void ws::ConfParser::parse_directory_list(ws::Server& server, ws::Token& token, std::stringstream& buffer) {

}

// void ws::ConfParser::parse_limit_except(ws::Server& server, ws::Token& token, std::stringstream& buffer) {
//   token << buffer;

//   for (; !token.is_endl(); token << buffer) {
//     if (token[token.length() - 1] == ';')
//       break;

//     server.set_limit_except(token.get_token());
//   }

//   server.set_limit_except(token.substr(0, token.length() - 1));
// }

/*
param: configuration file, current directory(which should be project root directory)
return: server vector, storing multiple server blocks
exception: throws if configure file has an error
description: parse configure file.
1. read file to stream
2. get word from stream
3. skips all whitespace, empty line
4. check server block syntax, throws if server header has an error
4 - 1. parse server block, throws if server block has an error
5. loop until stream's eof reached
6. returns server vector
*/
std::vector<ws::Server> ws::ConfParser::parse(const std::string& file, const std::string& curr_dir) const {
  std::stringstream buffer(this->open_file(file));

  std::string line;
  std::vector<ws::Server> ret;

  Token token;

  while (!buffer.eof()) {
    token << buffer;

    if (token.is_endl())
      continue;

    this->check_block_header(token, buffer, "server");

    ret.push_back(this->parse_server(token, buffer, curr_dir));
  }

  return ret;
}
