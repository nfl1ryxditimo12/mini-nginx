#include "HttpHeader.hpp"

ws::HttpHeader::HttpHeader()
: _content_length(0), _connection(""), _content_type(""), _transfer_encoding(""), _host(""), _date(""), \
	_accept(""), _accept_encoding(""), _accept_language(""), _referer(""), _expires(""), _retry_after("") {}

ws::HttpHeader::~HttpHeader() {}

/* =================================== */
/*                Getter               */
/* =================================== */

std::string::size_type ws::HttpHeader::get_content_length() const throw() {
	return _content_length;
}

std::string ws::HttpHeader::get_connection() const throw() {
	return _connection;
}

std::string ws::HttpHeader::get_content_type() const throw() {
	return _content_type;
}

std::string ws::HttpHeader::get_transfer_encoding() const throw() {
	return _transfer_encoding;
}

std::string ws::HttpHeader::get_host() const throw() {
	return _host;
}

std::string ws::HttpHeader::get_date() const throw() {
	return _date;
}

std::string ws::HttpHeader::get_accept() const throw() {
	return _accept;
}

std::string ws::HttpHeader::get_accept_encoding() const throw() {
	return _accept_encoding;
}

std::string ws::HttpHeader::get_accept_language() const throw() {
	return _accept_language;
}

std::string ws::HttpHeader::get_referer() const throw() {
	return _referer;
}

std::string ws::HttpHeader::get_expires() const throw() {
	return _expires;
}

std::string ws::HttpHeader::get_retry_after() const throw() {
	return _retry_after;
}

/* =================================== */
/*                Setter               */
/* =================================== */

void ws::HttpHeader::set_content_length(const std::string& var) throw() {
	std::string::size_type size = std::stoul(var); // c++11 func
	_content_length = size;
}

void ws::HttpHeader::set_connection(const std::string& var) throw() {
	_connection = var;
}

void ws::HttpHeader::set_content_type(const std::string& var) throw() {
	_content_type = var;
}

void ws::HttpHeader::set_transfer_encoding(const std::string& var) throw() {
	_transfer_encoding = var;
}

void ws::HttpHeader::set_host(const std::string& var) throw() {
	_host = var;
}

void ws::HttpHeader::set_date(const std::string& var) throw() {
	_date = var;
}

void ws::HttpHeader::set_accept(const std::string& var) throw() {
	_accept = var;
}

void ws::HttpHeader::set_accept_encoding(const std::string& var) throw() {
	_accept_encoding = var;
}

void ws::HttpHeader::set_accept_language(const std::string& var) throw() {
	_accept_language = var;
}

void ws::HttpHeader::set_referer(const std::string& var) throw() {
	_referer = var;
}

void ws::HttpHeader::set_expires(const std::string& var) throw() {
	_expires = var;
}

void ws::HttpHeader::set_retry_after(const std::string& var) throw() {
	_retry_after = var;
}
