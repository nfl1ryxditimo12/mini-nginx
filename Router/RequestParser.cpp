#include "RequestParser.hpp"

ws::RequestParser::RequestParser(): _fatal(NULL), _status(NULL), _request(NULL), _repository(NULL) {
  _header_parser.insert(header_parse_map_type::value_type("Host", &RequestParser::parse_host));
  _header_parser.insert(header_parse_map_type::value_type("Connection", &RequestParser::parse_connection));
  _header_parser.insert(header_parse_map_type::value_type("Content-Length", &RequestParser::parse_content_length));
  _header_parser.insert(header_parse_map_type::value_type("Content-Type", &RequestParser::parse_content_type));
  _header_parser.insert(header_parse_map_type::value_type("Transfer-Encoding", &RequestParser::parse_transfer_encoding));
}

ws::RequestParser::~RequestParser() {}

void  ws::RequestParser::init_client_data(client_value_type& client_data) {
  _fatal = &client_data.fatal;
  _status = &client_data.status;
  _request = &client_data.request;
  _repository = &client_data.repository;
}

int   ws::RequestParser::parse_request_message(const ws::Configure& conf, client_value_type& client_data, const char* message, const int read_size) {
  init_client_data(client_data);


}

void  ws::RequestParser::parse_host(const std::string& value) {
  std::string::size_type pos = value.find_first_of(":");

  if (!_server_name.empty()) {
    _client_data.status = BAD_REQUEST;
    return;
  }

  _server_name = value.substr(0, pos);
  if (inet_addr(_server_name.c_str()) != INADDR_NONE) //ip
    _server_name = "_";

  if (pos != std::string::npos)
    _port = ws::Util::stoul((value.substr(pos + 1, value.length())).c_str(), std::numeric_limits<u_int16_t>::max());
}

void  ws::RequestParser::parse_connection(const std::string& value) {
  _connection = value;
}

void  ws::RequestParser::parse_content_length(const std::string& value) {
  std::string::size_type content_length = ws::Util::stoul(value);

  if (content_length == value.npos) {
    _status = BAD_REQUEST;
    return;
  }

  _content_length = content_length;
}

void  ws::RequestParser::parse_content_type(const std::string& value) {
  //text/html; charset=utf-8
  std::string::size_type pos = value.find(';');

  _content_type = value.substr(0, pos);
}

void  ws::RequestParser::parse_transfer_encoding(const std::string& value) {
  _transfer_encoding = value;
  if (value == "chunked")
    _chunked = true;
  return;
}