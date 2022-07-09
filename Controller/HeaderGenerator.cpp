#include "HeaderGenerator.hpp"

#include <iostream>

#include <ctime>

#include "Define.hpp"
#include "Util.hpp"

std::string ws::HeaderGenerator::generate(unsigned int stat, std::string::size_type content_length) {
  std::string data;

  generate_start_line(data, stat);
  generate_representation_header(data, stat, content_length);
  generate_response_header(data);

  return data;
}

// start field
void ws::HeaderGenerator::generate_start_line(std::string& data, unsigned int stat) {
  data += "HTTP/1.1 " + ws::Util::ultos(stat) + "\r\n";
}

// representation field
void ws::HeaderGenerator::generate_representation_header(
  std::string& data, unsigned int stat, std::string::size_type content_length
) {
  generate_content_type_line(data);

  if (!((100 <= stat && stat < 200) || stat == NO_CONTENT))
    generate_content_length(data, content_length);
}

void ws::HeaderGenerator::generate_content_type_line(std::string& data) {
  data += "Content-Type: text/html; charset=UTF-8\r\n";
}

void ws::HeaderGenerator::generate_content_length(std::string& data, std::string::size_type content_length) {
  data += "Content-Length: " + ws::Util::ultos(content_length) + " bytes\r\n";
}


// response field
void ws::HeaderGenerator::generate_response_header(std::string& data) {
  generate_date_line(data);
  generate_server_line(data);
}

void ws::HeaderGenerator::generate_date_line(std::string& data) {
  std::time_t curr_time = std::time(NULL);
  std::string curr_time_str = std::asctime(std::gmtime(&curr_time));
  curr_time_str.erase(curr_time_str.end() - 1);

  data += "Date: " + curr_time_str + "\r\n";
}

void ws::HeaderGenerator::generate_server_line(std::string& data) {
  data += "Server: Webserv\r\n";
}
