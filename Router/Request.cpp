#include "Request.hpp"
#include "Token.hpp"

#include <map>
#include <iostream> // todo
#include <stdexcept>

ws::Request::Request(ws::Configure::listen_type listen)
: _listen(listen), _method(""), _request_uri(""), _http_version(""), _request_body("") {}

ws::Request::Request(const Request& cls)
: _listen(cls._listen), _method(cls._method), _request_uri(cls._request_uri), _http_version(cls._http_version), _request_body(cls._request_body) {}

ws::Request::~Request() {}

void	ws::Request::parse_request_body(ws::Token& token, std::stringstream& buffer) {
  token.rdall(buffer);

  for (size_t i = 0; i < token.length(); i++)
    _request_body.push_back(token[i]);
}

void	ws::Request::parse_request_header(ws::Token& token, std::stringstream& buffer) {
  std::string key;
  std::string value;

  /* request start line */
	_method = token.rdword(buffer);
	_request_uri = token.rdword(buffer);
	_http_version = token.rd_http_line(buffer);
  
  /* request header */
  for (token.rdword(buffer); !buffer.eof(); token.rdword(buffer)) {
    if (token == "\r\n")
      break;
    if (token.find(":") == token.npos || token[token.length() - 1] != ':')
      throw std::invalid_argument("Request: wrong header form: key");
    key = token.substr(0, token.length() - 1);
    value = token.rd_http_line(buffer).substr(1, token.length() - 1);
    _request_header.insert(std::pair<std::string, std::string>(key, value));
  }

  if (token != "\r\n")
    throw; // error response
}

void	ws::Request::parse_request_message(const char* message) {
  
  ws::Token token;
  std::stringstream buffer;

  buffer << message;

  if (!_request_header.size())
    parse_request_header(token, buffer);
  else
    parse_request_body(token, buffer);
}

/* getter */
std::string ws::Request::get_method() const throw() {
	return _method;
}

std::string ws::Request::get_uri() const throw() {
	return _request_uri;
}

std::string ws::Request::get_version() const throw() {
	return _http_version;
}

ws::Request::header_type ws::Request::get_request_header() const throw() {
	return _request_header;
}

std::string ws::Request::get_request_body() const throw() {
	return _request_body;
}


/* test */
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"

void	ws::Request::print_message() {
	std::cout << "\n" << YLW << "= TEST =========================================" << NC << "\n" << std::endl;

	std::cout << _method << " " << _request_uri << " HTTP/1.1\n" << std::endl;

	std::cout << "HEADER" << std::endl;
	for (std::map<std::string, std::string>::iterator it = _request_header.begin(); it != _request_header.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
	
	std::cout << std::endl;

	std::cout << "BODY" << std::endl;
	std::cout << _request_body << std::endl;

	std::cout << "\n" << YLW << "================================================" << NC << "\n" << std::endl;
}
