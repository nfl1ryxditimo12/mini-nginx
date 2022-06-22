#include "ConfParser.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <fstream>
#include <stdexcept>

#include <cctype>

#include "Util.hpp"

// initialize buffer with configure file
ws::ConfParser::ConfParser(const std::string& file, const std::string& curr_dir)
  : _buffer(this->open_file(file)), _root_dir(curr_dir) {}

ws::ConfParser::~ConfParser() {}

ws::Token& ws::ConfParser::get_token() throw() {
  return _token;
}

std::stringstream& ws::ConfParser::get_buffer() throw() {
  return _buffer;
}

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

  return std::string(buffer.str());
}

ws::Token& ws::ConfParser::rdword() {
  return _token.rdword(_buffer);
}




void ws::ConfParser::check_block_header(const std::string& block_name) {
  if (_token != block_name)
    throw std::invalid_argument("Configure: wrong " + block_name + " header");

  this->rdword();
  if (_token != '{')
    throw std::invalid_argument("Configure: wrong " + block_name + " header");

  this->rdword();
  if (_token != '\n')
    throw std::invalid_argument("Configure: wrong " + block_name + " header");  
}
#include <iostream>
ws::Server ws::ConfParser::parse_server() {
  ws::Server ret;

  std::string line;

  server_parser_func_map server_parser_func;

  this->init_server_parser_func(server_parser_func);

  while (!_buffer.eof()) {
    this->rdword();
    if (_token == "\n")
      continue;

    server_parser_func_map::iterator iter;

    iter = server_parser_func.find(_token.get_data());
    if (iter == server_parser_func.end())
      throw std::invalid_argument("Configure: wrong field key");
    (this->*iter->second)(ret);
    this->rdword();
    if (_token != "\n") {
      std::cout << _token.get_data() <<"asd" << std::endl;;
      throw std::invalid_argument("Configure: wrong field argument number");
    }


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
  server_parser_func.insert(server_parser_func_map::value_type("listen", &ConfParser::parse_listen));
  server_parser_func.insert(server_parser_func_map::value_type("server_name", &ConfParser::parse_server_name));
  server_parser_func.insert(
    server_parser_func_map::value_type("client_max_body_size", &ConfParser::parse_client_max_body_size)
  );
  server_parser_func.insert(server_parser_func_map::value_type("autoindex", &ConfParser::parse_autoindex));
  server_parser_func.insert(server_parser_func_map::value_type("root", &ConfParser::parse_root));
  server_parser_func.insert(server_parser_func_map::value_type("index", &ConfParser::parse_root));
}

// localhost: 127.0.0.1
static const int kLOCALHOST = 2130706433;

void ws::ConfParser::parse_listen(ws::Server& server) {
  listen_value_type value;

  this->rdword();

  if (_token[_token.length() - 1] != ';')
    throw std::invalid_argument("Configure: listen: `;' should appear at eol");

  _token.erase(_token.length() - 1, 1);

  if (_token.find(":") != _token.npos) {
    ConfParser::parse_listen_host(':', value);
    ConfParser::parse_listen_port(value);
    server.set_listen(value);
  } else if (_token.find(".") != _token.npos) {
    ConfParser::parse_listen_host(0, value);
    value.second = htons(80);
    server.set_listen(value);
  } else {
    ConfParser::parse_listen_port(value);
    value.first = htonl(INADDR_ANY);
    server.set_listen(value);
  }
}

void ws::ConfParser::parse_listen_host(char delim, listen_value_type& value) {
  ws::Token::size_type end;

  if (!_token.compare(0, 9, "localhost")) {
    end = 9;
    value.first = htonl(kLOCALHOST);
  } else {
    end = std::min(_token.find(":"), _token.length());
    value.first = inet_addr(_token.substr(0, end).c_str());
  }

  if ((_token[end] != delim) || (value.first == INADDR_NONE))
    throw std::invalid_argument("Configure: listen: wrong host");
}

void ws::ConfParser::parse_listen_port(listen_value_type& value) {
  if (!_token.length())
    throw std::invalid_argument("Configure: listen: need port");

  u_int32_t port = 0;

  Token::size_type i = _token.find(":");
  if (i == _token.npos)
    i = 0;
  else
    ++i;

  for (; _token[i]; ++i) {
    if (!std::isdigit(_token[i]))
      throw std::invalid_argument("Configure: listen: non numeric value in port");

    port *= 10;
    port += _token[i] - '0';
    if (port > UINT16_MAX)
      throw std::out_of_range("Configure: listen: port out of range");
  }

  value.second = htons(port);
}

void ws::ConfParser::parse_server_name(ws::Server& server) {
  this->rdword();

  for (
    Token::size_type pos = _token.find(";");
    !(_buffer.eof());
    this->rdword(), pos = _token.find(";")
  ) {
    if (pos != _token.npos) {
      if (pos != _token.length() - 1)
        throw std::invalid_argument("Configure: server_name: `;' should appear at eol");
      if (pos == 0)
        throw std::invalid_argument("Configure: server_name: `;' should appear at eol");
      server.set_server_name(_token.substr(0, pos - 1));
      break;
    } else {
      if (!_token.length())
        throw std::invalid_argument("Configure: server_name: invalid format");
      if (_token == "\n")
        throw std::invalid_argument("Configure: server_name: invalid format");
      server.set_server_name(_token.get_data());
    }
  }
}

void ws::ConfParser::parse_client_max_body_size(ws::Server& server) {
  if (server.get_client_max_body_size() != kCLIENT_MAX_BODY_SIZE_UNSET)
    throw std::invalid_argument("Configure: client_max_body_size: duplicated client_max_body_size");

  this->rdword();


  if ((_token.find(";") != _token.length() - 1) || (_token[0] == ';'))
    throw std::invalid_argument("Configure: client_max_body_size: `;' should appear at eol");

  unsigned long size = 0;

  for (Token::size_type i = 0; _token[i] != ';'; ++i) {
    if (!std::isdigit(_token[i]))
      throw std::invalid_argument("Congfigure: client_max_body_size: non numeric value");

    if (size >= kCLIENT_MAX_BODY_SIZE_LIMIT / 10)
      throw std::out_of_range("Configure: client_max_body_size: too large value");
    size *= 10;
    if (size >= kCLIENT_MAX_BODY_SIZE_LIMIT - (_token[i] - '0'))
      throw std::out_of_range("Configure: client_max_body_size: too large value");
    size += _token[i] - '0';
  }

  if (size >= kCLIENT_MAX_BODY_SIZE_LIMIT / 1024)
    throw std::out_of_range("Configure: client_max_body_size: too large value");

  server.set_client_max_body_size(size * 1024);
}

void ws::ConfParser::parse_autoindex(ws::Server& server) {
  if (server.get_autoindex() != kAUTOINDEX_UNSET)
    throw std::invalid_argument("Configure: autoindex: duplicated autoindex");

  this->rdword();

  if (_token.find(";") != _token.length() - 1)
    throw std::invalid_argument("Configure: autoindex: `;' should appear at eol");

  if (!_token.compare(0, std::max(_token.length() - 1, 2UL), "on"))
    server.set_autoindex(true);
  else if (!_token.compare(0, std::max(_token.length() - 1, 2UL), "off"))
    server.set_autoindex(false);
  else
    throw std::invalid_argument("Configure: autoindex: wrong value");
}

void ws::ConfParser::parse_root(ws::Server& server) {
  if (server.get_root().length())
    throw std::invalid_argument("Configure: root: duplicated root");

  this->rdword();

  if ((_token.find(";") != _token.length() - 1) || (_token[0] == ';'))
    throw std::invalid_argument("Configure: root: `;' should appear at eol");

  if (!_token.compare(0, std::max(_token.length() - 1, 4UL), "html"))
    server.set_root(_root_dir);
  else
    server.set_root(_token.substr(0, _token.length() - 1));
}

void ws::ConfParser::parse_index(ws::Server& server) {
  this->rdword();

  for (
    Token::size_type pos = _token.find(";");
    !(_buffer.eof());
    this->rdword(), pos = _token.find(";")
  ) {
    if (pos != _token.npos) {
      if (pos != _token.length() - 1)
        throw std::invalid_argument("Configure: index: `;' should appear at eol");
      if (pos == 0)
        throw std::invalid_argument("Configure: index: `;' should appear at eol");
      server.set_index(_token.substr(0, pos - 1));
      break;
    } else {
      if (!_token.length())
        throw std::invalid_argument("Configure: index: invalid format");
      if (_token == "\n")
        throw std::invalid_argument("Configure: index: invalid format");
      server.set_index(_token.get_data());
    }
  }
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
std::vector<ws::Server> ws::ConfParser::parse() {
  std::string line;
  std::vector<ws::Server> ret;

  while (!_buffer.eof()) {
    this->rdword();

    if (_token.is_endl())
      continue;

    this->check_block_header("server");

    ret.push_back(this->parse_server());
  }

  return ret;
}
