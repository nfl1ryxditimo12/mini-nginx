#include "RequestMessage.hpp"
#include "Token.hpp"

#include <map>
#include <iostream> // todo
// #include <istream>

ws::RequestMessage::RequestMessage(): _method(kNOTHING), _request_uri(""), _body("") {}

ws::RequestMessage::~RequestMessage() {}

void	ws::RequestMessage::parse_request_message(const std::string& message) {
	std::stringstream buffer;
	ws::Token	token;
	std::string tmp;
	int i = 0;

	// buffer << message;

	/* 나중에 모듈화: parse_start_line */
	/* ==================================== */
	// token << buffer;
	// if (token == "GET")
	// 	_method = kGET;
	// else if (token == "POST") 
	// 	_method = kPOST;
	// else if (token == "DELETE") 
	// 	_method = kDELETE; // 4로 할지 추후 의논
	// // else 인 경우 error response
	// token << buffer;
	// _request_uri = token.get_token();
	// if (_method == kNOTHING || (token << buffer) != "HTTP/1.1" || (token << buffer) != "\r\n")
	// 	return; // error response
	if (message.find("GET") != message.npos)
		_method = kGET;
	else if (message.find("POST") != message.npos)
		_method = kPOST;
	else if (message.find("DELETE") != message.npos)
		_method = kDELETE; // 4로 할지 추후 의논
	// _request_uri = message.substr(message.find_first_of("/"), message.find_first_of("HTTP") - 1);
	_request_uri = "/";
	for (; message[i] != '\r' && message[i + 1] != '\n'; i++) {}
	i += 2;
	/* ==================================== */

	/* Header parsing */
	/* ==================================== */
	while (message[i]) {
		std::string key, value;

		// std::getline(buffer, line);
		for (; message[i] && message[i] != ':'; i++)
			key.push_back(message[i]);
		for (i += 2; message[i] && message[i] != '\r'; i++)
			value.push_back(message[i]);
		_header.insert(std::pair<std::string, std::string>(key, value));
		// line.clear();
		if (message[i] == '\r' && message[i + 1] == '\n' && message[i + 2] == '\r' && message[i + 3] == '\n') {
			i += 4;
			break;
		}
		i += 2;
	}
	/* ==================================== */

	/* Body parsing */
	/* ==================================== */
	char *ptr;
	std::map<std::string, std::string>::iterator iter = _header.find("Content-Length");
	int content_length = 0;

	if (iter == _header.end())
		iter = _header.find("content-length");

	if (iter != _header.end()) {
		for (; message[i]; i++) {
			_body.push_back(message[i]);
		}
		content_length = static_cast<int>(std::strtod(iter->second.c_str(), &ptr));
		if (content_length != _body.length())
			return; // error response
	}
	
	/* ==================================== */
}

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