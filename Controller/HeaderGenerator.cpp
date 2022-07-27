#include "HeaderGenerator.hpp"

#include <iostream>

#include <ctime>

#include "Define.hpp"
#include "Util.hpp"

ws::HeaderGenerator::HeaderGenerator() throw() {}
ws::HeaderGenerator::~HeaderGenerator() {}

std::string ws::HeaderGenerator::generate(const client_value_type& client_data) {
  std::string data;

  generate_start_line(data, client_data.status);
  generate_representation(data, client_data);
  //representation: content-type, content-length, transfer-encoding
  generate_response(data, client_data);
  //response header: date, server, allow, location
  generate_connection(data, client_data);
  //connection header: connection
  if (client_data.repository.is_session())
    generate_cookie(data, client_data);

  return data;
}

// start field
void ws::HeaderGenerator::generate_start_line(std::string& data, unsigned int stat) {
  data += "HTTP/1.1 " + ws::Util::ultos(stat) + "\r\n";
}

// representation field
  //representation: content-type, content-length, transfer-encoding
void ws::HeaderGenerator::generate_representation(std::string& data, const client_value_type& client_data) {
  if (client_data.repository.get_method() == "HEAD")
    return;
  generate_content_type_line(data, client_data);
}

void ws::HeaderGenerator::generate_content_type_line(std::string& data, const client_value_type& client_data) {
  const std::string& content_type = client_data.repository.get_content_type();

  if (content_type.empty())
    data += "Content-Type: text\r\n";
  else
    data += "Content-Type: " + content_type + "\r\n";
}

// response field
  //response header: date, server, allow
void ws::HeaderGenerator::generate_response(std::string& data, const client_value_type& client_data) {
  generate_date_line(data);
  generate_server_line(data);
  generate_allow_line(data, client_data);
  generate_location_line(data, client_data);
}

void ws::HeaderGenerator::generate_date_line(std::string& data) {
  time_t curr_time = time(NULL);
  std::string curr_time_str = asctime(gmtime(&curr_time));
  curr_time_str.erase(curr_time_str.end() - 1);

  data += (std::string)"Date: " + curr_time_str + "\r\n";
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

void ws::HeaderGenerator::generate_location_line(std::string& data, const client_value_type& client_data) {
if (client_data.status < 400 && client_data.status >= 300 && client_data.repository.get_redirect().first > 0)
    data += ("Location: " + client_data.repository.get_redirect().second + "\r\n");
}

//connection field
  //connection header: connection
void ws::HeaderGenerator::generate_connection(std::string& data, const client_value_type& client_data) {
  generate_connection_line(data);
  (void) client_data;
}

void ws::HeaderGenerator::generate_connection_line(std::string& data) {
   data += "Connection: close\r\n";
}

void ws::HeaderGenerator::generate_cookie(std::string& data, const client_value_type& client_data) {
  data += "Set-Cookie: session_id=" + ws::Util::ultos(client_data.request.get_session_id()) + "\r\n";
}
