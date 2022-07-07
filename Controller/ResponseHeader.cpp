#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "modernize-use-equals-default"

#include "ResponseHeader.hpp"

#include <ctime>

#include "Define.hpp"
#include "Util.hpp"

ws::ResponseHeader::ResponseHeader() {}

ws::ResponseHeader::~ResponseHeader() {}

const std::string &ws::ResponseHeader::get_data() const throw() {
  return _data;
}

void ws::ResponseHeader::generate_data(unsigned int stat, std::string::size_type content_length) {
  this->generate_start_line(stat);
  this->generate_representation_header(stat, content_length);
  this->generate_response_header();
}


// start field
void ws::ResponseHeader::generate_start_line(unsigned int stat) {
  _data += "HTTP/1.1 " + ultoa(stat) + "\r\n";
}


// representation field
void ws::ResponseHeader::generate_representation_header(unsigned int stat, std::string::size_type content_length) {
  this->generate_content_type_line();

  if (!((100 <= stat && stat < 200) || stat == NO_CONTENT))
    this->generate_content_length(content_length);
}

void ws::ResponseHeader::generate_content_type_line() {
  _data += "Content-Type: text/html; charset=utf-8\r\n";
}

void ws::ResponseHeader::generate_content_length(std::string::size_type content_length) {
  _data += "Content-Length: " + ultoa(content_length) + " bytes\r\n";
}


// response field
void ws::ResponseHeader::generate_response_header() {
  this->generate_date_line();
  this->generate_server_line();
}

void ws::ResponseHeader::generate_date_line() {
  std::time_t curr_date = std::time(NULL);

  _data += static_cast<std::string>("Date: ") + std::asctime(std::gmtime(&curr_date)) + "\r\n";
}

void ws::ResponseHeader::generate_server_line() {
  _data += "Server: Webserv\r\n";
}

#pragma clang diagnostic pop
