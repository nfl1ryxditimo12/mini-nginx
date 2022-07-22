#include "ConfParser.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <fstream>
#include <stdexcept>

#include <cctype>

#include <sys/stat.h>

#include "Util.hpp"

// initialize buffer with configure file
ws::ConfParser::ConfParser(const std::string& file, const std::string& curr_dir)
  : _buffer(this->read_file(file)), _root_dir(curr_dir + "/www") {
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
  _location_parser.insert(location_parser_func_map::value_type("session", &ConfParser::parse_session));
  _location_parser.insert(location_parser_func_map::value_type("limit_except", &ConfParser::parse_limit_except));
  _location_parser.insert(location_parser_func_map::value_type("return", &ConfParser::parse_return));
  _location_parser.insert(location_parser_func_map::value_type("cgi", &ConfParser::parse_cgi));
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

  struct stat status;
  stat(file.c_str(), &status);
  if (!(S_IFREG & status.st_mode))
    throw std::invalid_argument("Configure: wrong file type");

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

void ws::ConfParser::check_location_header(std::string& dir) {
  if (_token != "location")
    throw std::invalid_argument("Configure: wrong location header");

  this->rdword();

  dir = _token;

  if (dir[0] != '/')
    throw std::invalid_argument("Configure: location header: location dir must start with `/'");

  this->rdword();

  if (_token != "{")
    throw std::invalid_argument("Configure: wrong location header");
}

bool ws::ConfParser::check_block_end() {
  if (_token.find("}") != _token.npos && _token != "}")
    throw std::invalid_argument("Configure: wrong block end");
  return true;
}

void ws::ConfParser::parse_server(ws::Server& server) {
  location_map_type location_map;
  ws::InnerOption option;

  while (!_buffer.eof()) {
    this->rdword();

    if (_token == "\n")
      continue;

    server_parser_iterator server_iter = _server_parser.find(_token);
    option_parser_iterator option_iter = _option_parser.find(_token);

    if (server_iter != _server_parser.end())
      (this->*server_iter->second)(server);
    else if (option_iter != _option_parser.end())
      (this->*option_iter->second)(option);
    else if (_token == "location") {
      std::string dir;
      ws::Location location;

      this->check_location_header(dir);
      location.set_block_name(dir);
      this->parse_location(location);

      location_map.insert(location_pair_type(dir, location));
    } else if (check_block_end())
      break;
    else
      throw std::invalid_argument("Configure: wrong field key");

    this->rdword();

    if (_token != "\n")
      throw std::invalid_argument("Configure: wrong field argument number");
  }

  server.set_option(option);
  this->set_default_server(server);
  this->set_default_location(server, location_map);
  server.set_location_map(location_map);
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
    server.add_listen(listen);
  } else if (_token.find(".") != _token.npos) {
    ConfParser::parse_listen_host(0, listen);
    listen.second = htons(80);
    server.add_listen(listen);
  } else {
    ConfParser::parse_listen_port(listen);
    listen.first = htonl(INADDR_ANY);
    server.add_listen(listen);
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


  Token::size_type i = _token.find(":");
  if (i == _token.npos)
    i = 0;
  else
    ++i;

  listen_type::second_type port = ws::Util::stoul(std::string(_token.c_str() + i), std::numeric_limits<uint16_t>::max());

  listen.second = htons(port);
}

void ws::ConfParser::parse_server_name(ws::Server& server) {
  this->rdword();

  std::string server_name;

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
      server_name = _token.substr(0, pos);
      if (ws::Util::is_valid_server_name(server_name))
        server.add_server_name(_token.substr(0, pos));
      break;
    } else {
      if (!_token.length())
        throw std::invalid_argument("Configure: server_name: invalid format");
      if (_token == "\n")
        throw std::invalid_argument("Configure: server_name: invalid format");
      if (ws::Util::is_valid_server_name(server_name))
        server.add_server_name(_token);
    }
  }

  server.add_server_name("_");
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

void ws::ConfParser::parse_session(ws::Location& location) {
  if (location.get_block_name() != "/session")
    throw std::invalid_argument("Configure: location: session: must be written in \"/session\" location block");

  this->rdword();

  if (_token.find(";") != _token.length() - 1)
    throw std::invalid_argument("Configure: location: session: `;' should appear at eol");

  if (!_token.compare(0, std::max(_token.length() - 1, 2UL), "on"))
    location.set_session(true);
  else if (!_token.compare(0, std::max(_token.length() - 1, 2UL), "off"))
    location.set_session(false);
  else
    throw std::invalid_argument("Configure: location: session: wrong value");
}

void ws::ConfParser::parse_limit_except(ws::Location& location) {
  const limit_except_vec_type& limit_except_vec = location.get_limit_except_vec();
  if (!limit_except_vec.empty())
    throw std::invalid_argument("Configure: location: limit_except: duplicated limit_except");

  std::string method;

  while (true) {
    this->rdword();
    ws::Location::limit_except_vec_type::size_type pos = _token.find(";");

    if (pos != _token.npos && (_token.back() != ';' || _token[0] == ';'))
        throw std::invalid_argument("Configure: location: limit_except: `;' should appear at eol");

    method = get_method(_token.substr(0, std::min(pos, _token.length())));

    for (limit_except_vec_type::const_iterator it = limit_except_vec.begin(); it != limit_except_vec.end(); ++it) {
      if (method == *it)
        throw std::invalid_argument("Configure: location: limit_except: duplicated method");
    }

    location.add_limit_except(method);

    if (pos != std::string::npos)
      break;
  }
}

void ws::ConfParser::parse_cgi(ws::Location &location) {
  this->rdword();

  ws::Location::cgi_type value;
  value.first = _token;

  this->rdword();

  if (_token.back() != ';')
    throw std::invalid_argument("Configure: location: cgi: `;' should appear at eol");

  value.second = Util::get_root_dir() + _token.substr(0, _token.length() - 1);

  if (access(value.second.c_str(), X_OK) == -1)
    throw std::invalid_argument("Configure: location: cgi: wrong cgi program path");

  location.add_cgi(value);
}

ws::ConfParser::limit_except_type ws::ConfParser::get_method(const std::string& method) const {
  static const std::string method_list[] = {
    "GET",
    "POST",
    "DELETE",
    "HEAD",
    "PUT"
  };

  for (std::size_t i = 0; ; ++i) {
    if (i > 4)
      throw std::invalid_argument("Configure: location: limit_except: wrong method");
    if (method == method_list[i])
      return std::string(method_list[i]);
  }
}

void ws::ConfParser::parse_return(ws::Location& location) {
  if (location.get_return().first != 0)
    throw std::invalid_argument("Configure: location: return: duplicated return");

  this->rdword();

  unsigned int code = static_cast<unsigned int>(ws::Util::stoul(_token, 999));

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
  else {
    option.set_root(Util::parse_relative_path(_root_dir + "/" + _token.substr(0, _token.length() - 1)));
  }
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
      option.add_index(_token.substr(0, pos));
      break;
    } else {
      if (!_token.length())
        throw std::invalid_argument("Configure: index: invalid format");
      if (_token == "\n")
        throw std::invalid_argument("Configure: index: invalid format");
      option.add_index(_token);
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
    option.add_error_page(ws::Server::error_page_type(error_code[i], file));
}

ws::ConfParser::error_page_type::first_type ws::ConfParser::parse_error_code() const {
  unsigned long ret;

  ret = ws::Util::stoul(_token, 599, 300);
  if (ret == std::numeric_limits<unsigned long>::max())
    throw std::invalid_argument("Configure: error_page: invalid error_code");

  return static_cast<error_page_type::first_type>(ret);
}


void ws::ConfParser::set_default_server(ws::Server& server) const {
    if (server.get_listen_vec().empty())
      server.add_listen(listen_type(htonl(INADDR_ANY), htons(80)));
    if (server.get_client_max_body_size() == kCLIENT_MAX_BODY_SIZE_UNSET)
      server.set_client_max_body_size(1024 * 1024);
    if (server.get_autoindex() == kAUTOINDEX_UNSET)
      server.set_autoindex(false);
    if (server.get_root().empty())
      server.set_root(_root_dir);
    if (server.get_index_set().empty())
      server.add_index(index_type("index.html"));
}

void ws::ConfParser::set_default_location(ws::Server& server, location_map_type& location_map) const {
  location_map.insert(location_map_type::value_type("/", Location()));

  for (location_map_type::iterator it = location_map.begin(); it != location_map.end(); ++it) {
    ws::Location& location = it->second;

    if (location.get_limit_except_vec().empty()) {
      location.add_limit_except("GET");
      location.add_limit_except("POST");
      location.add_limit_except("DELETE");
      location.add_limit_except("HEAD");
      location.add_limit_except("PUT");
    }

    if (location.get_client_max_body_size() == kCLIENT_MAX_BODY_SIZE_UNSET)
      location.set_client_max_body_size(server.get_client_max_body_size());
    if (location.get_autoindex() == kAUTOINDEX_UNSET)
      location.set_autoindex(server.get_autoindex());
    if (location.get_root().empty())
      location.set_root(server.get_root());
    if (location.get_index_set().empty())
      location.set_index(server.get_index_set());
  }
}

ws::ConfParser::server_finder_type ws::ConfParser::init_server_finder(const server_vec_type& server_vec) const {
  server_finder_type server_finder;

  for (server_vec_type::const_iterator serv_it = server_vec.begin(); serv_it != server_vec.end(); ++serv_it) {
    const listen_vec_type& listen_vec = serv_it->get_listen_vec();
    for (listen_vec_type::const_iterator listen_it = listen_vec.begin(); listen_it != listen_vec.end(); ++listen_it) {
      const server_name_vec_type& serv_name_vec = serv_it->get_server_name_vec();
      for (
        server_name_vec_type::const_iterator serv_name_it = serv_name_vec.begin();
        serv_name_it != serv_name_vec.end();
        ++serv_name_it
      ) {
        server_finder.insert(
          server_finder_type::value_type(server_finder_type::key_type(*listen_it, *serv_name_it), *serv_it)
        );
      }
    }
  }

  return server_finder;
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

    ws::Server server;
    this->parse_server(server);

    if (!server.get_listen_vec().empty())
      server_vec.push_back(server);
  }

  conf.set_server_vec(server_vec);
  conf.set_server_finder(this->init_server_finder(conf.get_server_vec()));
}
