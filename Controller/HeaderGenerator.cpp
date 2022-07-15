#include "HeaderGenerator.hpp"

#include <iostream>

#include <ctime>

#include "Define.hpp"
#include "Util.hpp"

std::string ws::HeaderGenerator::generate(const client_value_type& client_data, std::string::size_type content_length) {
  std::string data;

  generate_start_line(data, client_data.status);
  generate_representation_header(data, client_data, content_length);
  //representation: content-type, content-length, transfer-encoding
  generate_response_header(data, client_data);
  //response header: date, server, allow
  generate_connection_header(data, client_data);
  //connection header: connection

  return data;
}

// start field
void ws::HeaderGenerator::generate_start_line(std::string& data, unsigned int stat) {
  data += "HTTP/1.1 " + ws::Util::ultos(stat) + "\r\n";
}

// representation field
  //representation: content-type, content-length, transfer-encoding
void ws::HeaderGenerator::generate_representation_header(
  std::string& data, const client_value_type& client_data, std::string::size_type content_length
) {
  if (client_data.repository.get_method() == "HEAD")
    return;

  generate_content_type_line(data); // todo
  (void)client_data;
  (void) content_length; // todo: maybe not required
//  if (!((100 <= client_data.status && client_data.status < 200) || client_data.status == NO_CONTENT))
//    generate_content_length_line(data, content_length);

//  generate_transfer_encoding_line(data, client_data);
}

void ws::HeaderGenerator::generate_content_type_line(std::string& data) {
  data += "Content-Type: text/html; charset=UTF-8\r\n"; // todo
}

void ws::HeaderGenerator::generate_content_length_line(std::string& data, std::string::size_type content_length) {
  data += "Content-Length: " + ws::Util::ultos(content_length) + " bytes\r\n";
} //header -> body -> content-lenght

void ws::HeaderGenerator::generate_transfer_encoding_line(std::string& data, const client_value_type& client_data) {
  const std::string transfer_encoding = client_data.request.get_transfer_encoding();
  if (transfer_encoding == "chunked")
    data += "Transfer-Encoding: " + transfer_encoding + "\r\n";
}

//client_value_type& client_data
//stat 변경 가능


// response field
  //response header: date, server, allow
void ws::HeaderGenerator::generate_response_header(std::string& data, const client_value_type& client_data) {
  generate_date_line(data);
  generate_server_line(data);
  generate_allow_line(data, client_data);
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

void ws::HeaderGenerator::generate_allow_line(std::string& data, const client_value_type& client_data) {
  if (client_data.repository.get_status() != METHOD_NOT_ALLOWED)
    return;

  data += "Allow: ";

  const limit_except_vec_type& limit_except_vec = client_data.repository.get_location()->get_limit_except_vec();
  for (
    limit_except_vec_type::const_iterator it = limit_except_vec.begin();
    it != limit_except_vec.end();
    ++it
  )
    data += *it + ", ";

  data[data.length() - 2] = '\r';
  data[data.length() - 1] = '\n';
}

//connection field
  //connection header: connection
void ws::HeaderGenerator::generate_connection_header(std::string& data, const client_value_type& client_data) {
//  if (client_data.repository.get_connection) // todo
    generate_connection_line(data);
  (void) client_data; // todo
}

void ws::HeaderGenerator::generate_connection_line(std::string& data) {
//  data += "Connection: keep-alive\r\n";
   data += "Connection: close\r\n";
}
