#include "RequestMessage.hpp"
#include "Token.hpp"

#include <map>
#include <iostream> // todo
#include <stdexcept>

ws::RequestMessage::RequestMessage(): _method(""), _request_uri(""), _body(""), _http_version(""), _request_body_size(0) {}

ws::RequestMessage::~RequestMessage() {}

void	ws::RequestMessage::parse_request_message(const char* message, int buffer_size) {
	
	ws::Token token;
	std::stringstream buffer;

	_request_body_size += buffer_size;

	buffer << message;

	/*request line*/
	_method = token.rdword(buffer);
	_request_uri = token.rdword(buffer);
	_http_version = token.rd_http_line(buffer);

	/*request header*/
	std::string key;
	std::string value;

	for (token.rdword(buffer); !buffer.eof(); token.rdword(buffer)) {
		if (token == "\r\n")
			break;
		if (token.find(":") == token.npos || token[token.length() - 1] != ':')
			throw std::invalid_argument("RequestMessage: wrong header form: key"); //400 error : 잘못된 문법
		key = token.substr(0, token.length() - 1);
		value = token.rd_http_line(buffer).substr(1, token.length() - 1);
		_request_header.insert(std::pair<std::string, std::string>(key, value));
	}

	/*body*/
	_request_body = token.rdall(buffer);
	buffer.clear();
}

/* getter */
std::string ws::RequestMessage::get_method() const throw() {
	return _method;
}

std::string ws::RequestMessage::get_uri() const throw() {
	return _request_uri;
}

std::string ws::RequestMessage::get_version() const throw() {
	return _http_version;
}

ws::RequestMessage::header_type ws::RequestMessage::get_request_header() const throw() {
	return _request_header;
}

std::string::size_type ws::RequestMessage::get_request_body_size() const throw() {
	return _request_body_size;
}


/* test */
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"

void	ws::RequestMessage::print_message() {
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
