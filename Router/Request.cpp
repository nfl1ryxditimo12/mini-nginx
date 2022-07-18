#include "Request.hpp"

/* 지울거임 */
#include <iostream>
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"

#include "Repository.hpp"

// todo member variable initialize

ws::Request::Request(const ws::Configure::listen_type& listen): _listen(listen) {
  _eof = false;

  _request_body_size = 0;

  _content_length = std::numeric_limits<std::size_t>::max();

  _chunked = false;
  _chunked_line_type = false;
  _chunked_eof = false;
  _chunked_byte = std::string::npos;
  _client_max_body_size = 0;
  _is_header = true;

  _request_message_offset = 0;
  _request_body_offset = 0;
}

ws::Request::Request(const Request& cls): _listen(cls._listen) {
  _eof = cls._eof;

  _method = cls._method;
  _request_uri = cls._request_uri;
  _request_uri_query = cls._request_uri_query;
  _http_version = cls._http_version;

  _request_header = cls._request_header;
  _request_body = cls._request_body;
  _request_body_size = cls._request_body_size;

  _content_length = cls._content_length;
  _content_type = cls._content_type;
  _server_name = cls._server_name;
  _port = cls._port;
  _connection = cls._connection;
  _transfer_encoding = cls._transfer_encoding;

  _chunked = cls._chunked;
  _chunked_line_type = cls._chunked_line_type;
  _chunked_byte = cls._chunked_byte;
  _client_max_body_size = cls._client_max_body_size;
  _is_header = cls._is_header;

  _request_message_offset = cls._request_message_offset;
  _request_body_offset = cls._request_body_offset;
//  _buffer = cls._buffer; todo
}

ws::Request::~Request() {}

/* ==================================== */
/*                Getter                */
/* ==================================== */

std::stringstream& ws::Request::r_buffer() throw() {
  return _buffer;
}

/*
  _eof
  case false: In progress request message parsing
  case  true: Request message parsing is done
*/
bool ws::Request::get_eof() const throw() {
  return _eof;
}

const std::string& ws::Request::get_method() const throw() {
	return _method;
}

const std::string& ws::Request::get_uri() const throw() {
	return _request_uri;
}

const ws::Request::query_map_type& ws::Request::get_uri_query() const throw() {
	return _request_uri_query;
}

const std::string& ws::Request::get_version() const throw() {
	return _http_version;
}

const ws::Request::header_map_type& ws::Request::get_request_header() const throw() {
	return _request_header;
}

ws::Request::body_list_type& ws::Request::get_request_body() throw() {
	return _request_body;
}

const size_t& ws::Request::get_request_body_size() const throw() {
  return _request_body_size;
}

const ws::Request::listen_type& ws::Request::get_listen() const throw() {
  return _listen;
}

const std::string::size_type& ws::Request::get_content_length() const throw() {
  return _content_length;
}

const std::string& ws::Request::get_content_type() const throw() {
  return _content_type;
}

const std::string& ws::Request::get_server_name() const throw() {
  return _server_name;
}

const u_int16_t& ws::Request::get_port() const throw() {
  return _port;
}

const std::string& ws::Request::get_connection() const throw() {
  return _connection;
}

const std::string& ws::Request::get_transfer_encoding() const throw() {
  return _transfer_encoding;
}

bool ws::Request::get_is_header() const throw() {
  return _is_header;
}

bool ws::Request::get_chunked() const throw() {
  return _chunked;
}

bool ws::Request::get_chunked_eof() const throw() {
  return _chunked_eof;
}

bool ws::Request::get_chunked_line_type() const throw() {
  return _chunked_line_type;
}

const size_t& ws::Request::get_chunked_byte() const throw() {
  return _chunked_byte;
}

const size_t& ws::Request::get_client_max_body_size() const throw() {
  return _client_max_body_size;
}

const int& ws::Request::get_request_message_offset() const throw() {
  return _request_message_offset;
}

const size_t& ws::Request::get_request_body_offset() const throw() {
  return _request_body_offset;
}


/* ==================================== */
/*                Setter                */
/* ==================================== */

void ws::Request::set_eof(bool eof) throw() {
  _eof = eof;
}

void ws::Request::set_method(const std::string& method) throw() {
  _method = method;
}

void ws::Request::set_request_uri(const std::string& request_uri) throw() {
  _request_uri = request_uri;
}

void ws::Request::set_request_uri_query(const query_pair_type& request_uri_query) throw() {
  _request_uri_query.insert(request_uri_query);
}

void ws::Request::set_http_version(const std::string& http_version) throw() {
  _http_version = http_version;
}


void ws::Request::set_request_header(const header_pair_type& request_header) throw() {
  _request_header.insert(request_header);
}

void ws::Request::set_request_body(const char* request_body, size_t start, size_t size) throw() {
  const char* body = request_body + start;

  _request_body_size += size;
  _request_body.push_back(body_list_type::value_type(body, size));
}

void ws::Request::set_request_body_size(int size) throw() {
  _request_body_size = size;
}

void ws::Request::add_request_body_size(int size) throw() {
  _request_body_size += size;
}

void ws::Request::set_listen(const listen_type& listen) throw() {
  _listen = listen;
}


void ws::Request::set_content_length(const size_t& content_length) throw() {
  _content_length = content_length;
}

void ws::Request::set_content_type(const std::string& content_type) throw() {
  _content_type = content_type;
}

void ws::Request::set_server_name(const std::string& server_name) throw() {
  _server_name = server_name;
}

void ws::Request::set_port(const u_int16_t& port) throw() {
  _port = port;
}

void ws::Request::set_connection(const std::string& connection) throw() {
  _connection = connection;
}

void ws::Request::set_transfer_encoding(const std::string& transfer_encoding) throw() {
  _transfer_encoding = transfer_encoding;
}


void ws::Request::set_is_header(bool is_header) throw() {
  _is_header = is_header;
}

void ws::Request::set_chunked(bool chunked) throw() {
  _chunked = chunked;
}

void ws::Request::set_chunked_eof(bool chunked_eof) throw() {
  _chunked_eof = chunked_eof;
}

void ws::Request::set_chunked_line_type(bool chunked_line_type) throw() {
  _chunked_line_type = chunked_line_type;
}

void ws::Request::set_chunked_byte(const size_t& chunked_byte) throw() {
  _chunked_byte = chunked_byte;
}

void ws::Request::add_chunked_byte(const int& chunked_byte) throw() {
  _chunked_byte += chunked_byte;
}

void ws::Request::set_client_max_body_size(const size_t& client_max_body_size) throw() {
  _client_max_body_size = client_max_body_size;
}

void ws::Request::set_request_message_offset(const size_t& request_message_offset) throw() {
  _request_message_offset = request_message_offset;
}

void ws::Request::set_request_body_offset(const size_t& request_body_offset) throw() {
  _request_body_offset = request_body_offset;
}

void ws::Request::add_request_message_offset(const int& request_message_offset) throw() {
  _request_message_offset += request_message_offset;
}

void ws::Request::add_request_body_offset(const size_t& request_body_offset) throw() {
  _request_body_offset += request_body_offset;
}


//todo: print test
void ws::Request::test() {
  std::cout << "host: _: " << _listen.first << ", port: " << _listen.second << std::endl;
  std::cout << "eof: " << _eof << std::endl;
  std::cout << "method: " << _method << std::endl;
  std::cout << "request_uri: " << _request_uri << std::endl;
  std::cout << "request_uri_query:" << std::endl;
  for (query_map_type::iterator it = _request_uri_query.begin(); it != _request_uri_query.end(); ++it) {
    std::cout << "  Key: " << it->first << ", Value: " << it->second << std::endl;
  }
  std::cout << "http version: " << _http_version << std::endl;
  std::cout << "content length: " << _content_length << std::endl;
  std::cout << "content type: " << _content_type << std::endl;
  std::cout << "server name: " << _server_name << std::endl;
  std::cout << "run_server: " << _connection << std::endl;
  std::cout << "transfer encoding: " << _transfer_encoding << std::endl;

  std::cout << "_chunked: " << (_chunked ? "true" : "false") << std::endl;
  std::cout << "_chunked_line_type: " << _chunked_line_type << std::endl;
  std::cout << "_chunked_byte: " << _chunked_byte << std::endl;
  std::cout << "_client_max_body_size: " << _client_max_body_size << std::endl;
  
  std::cout << "\nHeader:" << std::endl;
  for (header_map_type::iterator it = _request_header.begin(); it != _request_header.end(); ++it)
    std::cout << RED << "> " << NC << it->first << " " << it->second << std::endl;
  std::cout << "\nBody:" << std::endl;
  for (body_list_type::iterator it = _request_body.begin(); it != _request_body.end(); ++it)
    std::cout << it->first;
  std::cout << std::endl;
}
