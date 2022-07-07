#include "ResponseHeader.hpp"

#include <iostream>

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
  _data += "HTTP/1.1 " + ws::ultoa(stat) + "\r\n";
}

// representation field
void ws::ResponseHeader::generate_representation_header(unsigned int stat, std::string::size_type content_length) {
  this->generate_content_type_line();

  if (!((100 <= stat && stat < 200) || stat == NO_CONTENT))
    this->generate_content_length(content_length);
}

void ws::ResponseHeader::generate_content_type_line() {
  _data += "Content-Type: text/html; charset=UTF-8\r\n";
}

void ws::ResponseHeader::generate_content_length(std::string::size_type content_length) {
  _data += "Content-Length: " + ws::ultoa(content_length) + " bytes\r\n";
}


// response field
void ws::ResponseHeader::generate_response_header() {
  this->generate_date_line();
  this->generate_server_line();
}

void ws::ResponseHeader::generate_date_line() {
  std::time_t curr_time = std::time(NULL);
  std::string curr_time_str = std::asctime(std::gmtime(&curr_time));
  curr_time_str.erase(curr_time_str.end() - 1);

//  Date: Thu Jul  7 07:14:21 2022
//  Date: Tue, 15 Nov 1994 08:12:31 GMT
  _data += "Date: " + curr_time_str + "\r\n";
}

void ws::ResponseHeader::generate_server_line() {
  _data += "Server: Webserv\r\n";
}

void ws::ResponseHeader::print_data() const {
  std::cout << _data << std::endl;
}
