#include "ConfParser.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <fstream>
#include <stdexcept>

#include <cctype>

#include "Util.hpp"

// initialize buffer with configure file
ws::ConfParser::ConfParser(const std::string& file, const std::string& curr_dir)
  : _buffer(this->read_file(file)), _root_dir(curr_dir) {
  init_server_parser();
  init_location_parser();
  init_option_parser();
}

ws::ConfParser::~ConfParser() {}

void ws::ConfParser::init_server_parser() {
  _server_parser.insert(server_parser_func_map::value_type("listen", &ConfParser::parse_listen));
  _server_parser.insert(server_parser_func_map::value_type("server_name", &ConfParser::parse_server_name));
}

void ws::ConfParser::init_location_parser() {
  _location_parser.insert(location_parser_func_map::value_type("limit_except", &ConfParser::parse_limit_except));
  _location_parser.insert(location_parser_func_map::value_type("return", &ConfParser::parse_return));
}

void ws::ConfParser::init_option_parser() {
  _option_parser.insert(option_parser_func_map::value_type("autoindex", &ConfParser::parse_autoindex));
  _option_parser.insert(option_parser_func_map::value_type("root", &ConfParser::parse_root));
  _option_parser.insert(option_parser_func_map::value_type("index", &ConfParser::parse_index));
  _option_parser.insert(
    option_parser_func_map::value_type("client_max_body_size", &ConfParser::parse_client_max_body_size)
  );
  _option_parser.insert(option_parser_func_map::value_type("error_page", &ConfParser::parse_error_page));
}

ws::Token& ws::ConfParser::get_token() throw() {
  return _token;
}

std::stringstream& ws::ConfParser::get_buffer() throw() {
  return _buffer;
}

std::string ws::ConfParser::read_file(const std::string& file) const {
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




void ws::ConfParser::check_server_header() {
  if (_token != "server")
    throw std::invalid_argument("Configure: wrong server header");

  this->rdword();
  if (_token != "{")
    throw std::invalid_argument("Configure: wrong server header");

  this->rdword();
  if (_token != "\n")
    throw std::invalid_argument("Configure: wrong server header");
}

void ws::ConfParser::check_location_header(std::string& dir, ws::Location& location) {
  if (_token != "location")
    throw std::invalid_argument("Configure: wrong location header");

  this->rdword();

  std::string cgi;
  std::string temp(_token);

  this->rdword();

  if (_token != "{") {
    dir = _token;
    cgi = temp;
    this->rdword();
  } else
    dir = temp;

  location.set_cgi(cgi);

  if (_token != "{")
    throw std::invalid_argument("Configure: wrong location header");
}

bool ws::ConfParser::check_block_end() {
  if (_token.find("}") != _token.npos && _token != "}")
    throw std::invalid_argument("Configure: wrong block end");
  return true;
}



ws::Server ws::ConfParser::parse_server() {
  ws::Server ret;
  location_map_type location_map;
  ws::InnerOption option;

  while (!_buffer.eof()) {
    this->rdword();

    if (_token == "\n")
      continue;

    server_parser_iterator server_iter = _server_parser.find(_token);
    option_parser_iterator option_iter = _option_parser.find(_token);

    if (server_iter != _server_parser.end())
      (this->*server_iter->second)(ret);
    else if (option_iter != _option_parser.end())
      (this->*option_iter->second)(option);
    else if (_token == "location") {
      std::string dir;
      ws::Location location;

      this->check_location_header(dir, location);
      this->parse_location(location);

      this->set_default_location(location);

      location_map.insert(location_pair_type(dir, location));
    } else if (check_block_end())
      break;
    else
      throw std::invalid_argument("Configure: wrong field key");

    this->rdword();

    if (_token != "\n")
      throw std::invalid_argument("Configure: wrong field argument number");
  }

  ret.set_location_map(location_map);
  ret.set_option(option);
  this->set_default_server(ret);

  return ret;
}

void ws::ConfParser::parse_location(ws::Location& location) {
  ws::InnerOption option;

  while (!_buffer.eof()) {
    this->rdword();
    if (_token == "\n")
      continue;

    location_parser_iterator location_iter = _location_parser.find(_token);
    option_parser_iterator option_iter = _option_parser.find(_token);

    if (location_iter != _location_parser.end())
      (this->*location_iter->second)(location);
    else if (option_iter != _option_parser.end())
      (this->*option_iter->second)(option);
    else if (this->check_block_end())
      break;
    else
      throw std::invalid_argument("Configure: location: wrong field key");

    this->rdword();

    if (_token != "\n")
      throw std::invalid_argument("Configure: location: wrong field argument number");
  }

  location.set_option(option);
  this->set_default_location(location);
}

// localhost: 127.0.0.1
static const int kLOCALHOST = 2130706433;

void ws::ConfParser::parse_listen(ws::Server& server) {
  listen_type listen;

  this->rdword();

  if (_token.back() != ';')
    throw std::invalid_argument("Configure: listen: `;' should appear at eol");

  _token.erase(_token.length() - 1, 1);

  if (_token.find(":") != _token.npos) {
    ConfParser::parse_listen_host(':', listen);
    ConfParser::parse_listen_port(listen);
    server.set_listen_vec(listen);
  } else if (_token.find(".") != _token.npos) {
    ConfParser::parse_listen_host(0, listen);
    listen.second = htons(80);
    server.set_listen_vec(listen);
  } else {
    ConfParser::parse_listen_port(listen);
    listen.first = htonl(INADDR_ANY);
    server.set_listen_vec(listen);
  }
}

void ws::ConfParser::parse_listen_host(char delim, listen_type& listen) {
  ws::Token::size_type end;

  if (!_token.compare(0, 9, "localhost")) {
    end = 9;
    listen.first = htonl(kLOCALHOST);
  } else {
    end = std::min(_token.find(":"), _token.length());
    listen.first = inet_addr(_token.substr(0, end).c_str());
  }

  if ((_token[end] != delim) || (listen.first == INADDR_NONE))
    throw std::invalid_argument("Configure: listen: wrong host");
}

void ws::ConfParser::parse_listen_port(listen_type& listen) {
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
      throw std::invalid_argument("Configure: listen: non numeric listen in port");

    port *= 10;
    port += _token[i] - '0';
    if (port > UINT16_MAX)
      throw std::out_of_range("Configure: listen: port out of range");
  }

  listen.second = htons(port);
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
      server.set_server_name_vec(_token.substr(0, pos));
      break;
    } else {
      if (!_token.length())
        throw std::invalid_argument("Configure: server_name: invalid format");
      if (_token == "\n")
        throw std::invalid_argument("Configure: server_name: invalid format");
      server.set_server_name_vec(_token);
    }
  }
}

void ws::ConfParser::parse_client_max_body_size(ws::InnerOption& option) {
  if (option.get_client_max_body_size() != kCLIENT_MAX_BODY_SIZE_UNSET)
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

  option.set_client_max_body_size(size * 1024);
}

void ws::ConfParser::parse_limit_except(ws::Location& location) {
  ws::Location::limit_except_map_type limit_except(location.get_limit_except_map());

  for (limit_except_type::iterator it = limit_except.begin(); it != limit_except.end(); ++it) {
    if (it->second != -1)
      throw std::invalid_argument("Configure: location: limit_except: duplicated limit_except");
  }

  std::string method;

  while (1) {
    this->rdword();
    ws::Location::limit_except_map_type::size_type pos = _token.find(";");

    if (pos != _token.npos && (_token[pos] != ';' || _token[0] == ';'))
        throw std::invalid_argument("Configure: location: limit_except: wrong format");

    method = get_method(_token.substr(0, std::min(pos, _token.length())));

    if (limit_except.find(method)->second != -1)
      throw std::invalid_argument("Configure: location: limit_except: duplicated method");

    location.set_limit_except(method, true);

    if (pos != _token.npos)
      break;
  }
}

std::string ws::ConfParser::get_method(const std::string& method) const {
  static const std::string method_list[] = {
    "GET",
    "POST",
    "DELETE",
    "HEAD"
  };

  for (std::size_t i = 0; ; ++i) {
    if (i > 3)
      throw std::invalid_argument("Configure: location: limit_except: wrong method");
    if (method == method_list[i])
      return std::string(method_list[i]);
  }
}

void ws::ConfParser::parse_return(ws::Location& location) {
  if (location.get_return().first != 0)
    throw std::invalid_argument("Configure: location: return: duplicated return");

  this->rdword();

  int code = 0;
  for (std::size_t i = 0; _token[i]; ++i) {
    if (!std::isdigit(_token[i]))
      throw std::invalid_argument("Configure: return: non numeric value in code");

    code *= 10;
    code += _token[i] - '0';


    if (code > 999)
      throw std::out_of_range("Configure: return: invalid return code");
  }

  this->rdword();

  if (_token[0] == ';' || _token.back() != ';')
    throw std::invalid_argument("Configure: return: `;' should appear at eol");

  location.set_return(ws::Location::return_type(code, _token.substr(0, _token.length() - 1)));
}

void ws::ConfParser::parse_autoindex(ws::InnerOption& option) {
  if (option.get_autoindex() != kAUTOINDEX_UNSET)
    throw std::invalid_argument("Configure: autoindex: duplicated autoindex");

  this->rdword();

  if (_token.find(";") != _token.length() - 1)
    throw std::invalid_argument("Configure: autoindex: `;' should appear at eol");

  if (!_token.compare(0, std::max(_token.length() - 1, 2UL), "on"))
    option.set_autoindex(true);
  else if (!_token.compare(0, std::max(_token.length() - 1, 2UL), "off"))
    option.set_autoindex(false);
  else
    throw std::invalid_argument("Configure: autoindex: wrong value");
}

void ws::ConfParser::parse_root(ws::InnerOption& option) {
  if (option.get_root().length())
    throw std::invalid_argument("Configure: root: duplicated root");

  this->rdword();

  if ((_token.find(";") != _token.length() - 1) || (_token[0] == ';'))
    throw std::invalid_argument("Configure: root: `;' should appear at eol");

  if (!_token.compare(0, std::max(_token.length() - 1, 4UL), "html"))
    option.set_root(_root_dir);
  else
    option.set_root(_token.substr(0, _token.length() - 1));
}

void ws::ConfParser::parse_index(ws::InnerOption& option) {
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
      option.set_index(_token.substr(0, pos));
      break;
    } else {
      if (!_token.length())
        throw std::invalid_argument("Configure: index: invalid format");
      if (_token == "\n")
        throw std::invalid_argument("Configure: index: invalid format");
      option.set_index(_token);
    }
  }
}

void ws::ConfParser::parse_error_page(ws::InnerOption& option) {
  this->rdword();

  std::vector<int> error_code;
  std::string file;

  for (
    Token::size_type pos = _token.find(";");
    !(_buffer.eof());
    this->rdword(), pos = _token.find(";")
  ) {
    if (pos != _token.npos) {
      if (pos != _token.length() - 1)
        throw std::invalid_argument("Configure: error_page: `;' should appear at eol");
      if (pos == 0)
        throw std::invalid_argument("Configure: error_page: `;' should appear at eol");
      file = _token.substr(0, pos);
      break;
    } else {
      if (!_token.length())
        throw std::invalid_argument("Configure: error_page: invalid format");
      if (_token == "\n")
        throw std::invalid_argument("Configure: error_page: invalid format");
      error_code.push_back(this->parse_error_code());
    }
  }

  if (error_code.empty())
    throw std::invalid_argument("Configure: error_pgae: need error code");

  for (std::vector<int>::size_type i = 0; i < error_code.size(); ++i)
    option.set_error_page_map(ws::Server::error_page_type(error_code[i], file));
}

int ws::ConfParser::parse_error_code() const {
  std::string curr(_token);
  int ret = 0;

  for (std::string::size_type i = 0; (ret < 600) && curr[i]; ++i) {
    if (!std::isdigit(curr[i]))
      throw std::invalid_argument("Configure: error_page: non numeric in error code");
    ret *= 10;
    ret += curr[i] - '0';
  }

  if (ret < 300 || ret > 599)
    throw std::out_of_range("Configure: error_page: error code range must be between 300 and 599");

  return ret;
}


void ws::ConfParser::set_default_server(ws::Server& server) {
    if (server.get_listen_vec().empty())
      server.set_listen_vec(listen_type(htonl(INADDR_ANY), htons(80)));
    if (server.get_client_max_body_size() == kCLIENT_MAX_BODY_SIZE_UNSET)
      server.set_client_max_body_size(1024 * 1024);
    if (server.get_autoindex() == kAUTOINDEX_UNSET)
      server.set_autoindex(false);
    if (server.get_root().empty())
      server.set_root(_root_dir);
    if (server.get_index_vec().empty())
      server.set_index(index_type("index.html"));
}

// should do after parsing
void ws::ConfParser::set_default_location(ws::Location& location) {
  if (location.get_limit_except_map().empty()) {
    location.set_limit_except("GET", true);
    location.set_limit_except("POST", true);
    location.set_limit_except("DELETE", true);
    location.set_limit_except("HEAD", true);
  }
  if (location.get_client_max_body_size() == kCLIENT_MAX_BODY_SIZE_UNSET)
    location.set_client_max_body_size(1024 * 1024);
  if (location.get_autoindex() == kAUTOINDEX_UNSET)
    location.set_autoindex(false);
  if (location.get_root().empty())
    location.set_root(_root_dir);
  if (location.get_index_vec().empty())
    location.set_index(index_type("index.html"));
}

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
void ws::ConfParser::parse(ws::Configure& conf) {
  server_vec_type server_vec;

  while (!_buffer.eof()) {
    this->rdword();

    if (_token == "\n" || _token.empty())
      continue;

    this->check_server_header();

    server_vec.push_back(this->parse_server());
  }

  conf.set_server_vec(server_vec);
}
