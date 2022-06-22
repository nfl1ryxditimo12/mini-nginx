#include "RequestMessage.hpp"
#include "Token.hpp"

#include <map>
#include <iostream> // todo
#include <stdexcept>

ws::RequestMessage::RequestMessage(): _method(kNOTHING), _request_uri(""), _body(""), _http_version(""), _request_size(0) {}

ws::RequestMessage::~RequestMessage() {}

void	ws::RequestMessage::parse_request_message(const char* message, int buffer_size) {
	
	ws::Token token;
	std::stringstream buffer;

	_request_size += buffer_size;

	buffer << message;

	/*request line*/
	token.rdword(buffer);
	if (token == "GET")
		_method = kGET;
	else if (token == "POST")
		_method = kPOST;
	else if (token == "DELETE")
		_method = kDELETE;
	//else -> kNOTHING
	_request_uri = token.rdword(buffer).get_data();
	_http_version = token.rd_http_line(buffer).get_data();
	
	/*request header*/
	std::string key;
	std::string value;

	for (token.rdword(buffer); !buffer.eof(); token.rdword(buffer)) {
	if (token == "\r\n")
		break;
	if (token.find(":") == token.npos || token[token.length() - 1] != ':')
		throw std::invalid_argument("RequestMessage: wrong header form: key");
	key = token.get_data();
	value = token.rd_http_line(buffer).substr(1, token.length() - 1);
	_header.insert(std::pair<std::string, std::string>(key, value));
	}
	/*body*/

	_body = token.rdall(buffer).get_data();

	buffer.clear();
}

/* test */
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"

void	ws::RequestMessage::print_message() {
	std::cout << "\n" << YLW << "= TEST =========================================" << NC << "\n" << std::endl;

	std::cout << (_method == kGET
									? "GET"
									: (_method == kPOST
										? "POST"
										: _method == kDELETE
											? "DELETE"
											: "NOTHING")) << " ";
	std::cout << _request_uri << " HTTP/1.1\n" << std::endl;

	std::cout << "Header:" << std::endl;
	for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
	
	std::cout << std::endl;

	std::cout << "Body:" << std::endl;
	std::cout << _body << std::endl;

	std::cout << "\n" << YLW << "================================================" << NC << "\n" << std::endl;
}