#include "Request.hpp"

/* 지울거임 */
#include <iostream>
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"

#include "Repository.hpp"

ws::Request::Request(const ws::Configure::listen_type& listen)
  : _listen(listen), _eof(false), _content_length(std::numeric_limits<std::size_t>::max()),
    _status(0), _chunked(false), _chunked_line_type(false), _chunked_byte(std::string::npos), _client_max_body_size(0), _is_header(true),
    _token(), _buffer() {
  insert_require_header_field();
}

ws::Request::Request(const Request& cls) {
  _listen = cls._listen;
  _eof = cls._eof;

  _method = cls._method;
  _request_uri = cls._request_uri;
  _request_uri_query = cls._request_uri_query;
  _http_version = cls._http_version;

  _request_header = cls._request_header;
  _request_body = cls._request_body;

  _content_length = cls._content_length;
  _content_type = cls._content_type;
  _server_name = cls._server_name;
  _port = cls._port;
  _connection = cls._connection;
  _transfer_encoding = cls._transfer_encoding;

  _header_parser = cls._header_parser;
  _status = cls._status;
  _chunked = cls._chunked;
  _chunked_line_type = cls._chunked_line_type;
  _chunked_byte = cls._chunked_byte;
  _client_max_body_size = cls._client_max_body_size;
  _is_header = cls._is_header;
}

ws::Request::~Request() {}

/*
  chunked인 경우 어떻게 처리할까?
  chunk-start-line 파싱 -> chunked-content push_back 형식?
  만약 chunked-content-length보다 chunked-content가 적게 들어오면?
*/
void	ws::Request::parse_request_chunked_body() {
  while (!_buffer.eof() && !_status) {
    rd_http_line(); // todo: 0\r\n 으로 들어오는 경우 \r\n 잘라주는 로직 필요

    if (_token.length() < 2 || _token.compare(_token.length() - 2, 2, "\r\n")) {
      _buffer.clear();
      _buffer << _token;
      return;
    }

    _token.erase(_token.length() - 2, 2);

    if (!_chunked_line_type) {
      _chunked_byte = ws::Util::stoul(_token, std::numeric_limits<unsigned long>::max(), 0, "0123456789ABCDE");
      _chunked_line_type = 1;
      if (_chunked_byte == std::string::npos)
        _status = BAD_REQUEST;
    }
    else {
      if (_chunked_byte == 0 && _token.length() == 0) {
        _eof = true;
        return;
      }
      for (std::string::size_type i = 0; i < _token.length(); ++i, --_chunked_byte) {
        if (_chunked_byte == std::string::npos) {
          _status = BAD_REQUEST;
          return;
        }

        if (_request_body.length() == _client_max_body_size) {
          _status = 413;
          return;
        }
        _request_body.push_back(_token[i]);
      }
      if (_chunked_byte == 0)
        _chunked_line_type = 0;
    }
  }

  if (_status)
    _eof = true;
}

/*
  정의되어있는 Content-Length 값보다 적게 파싱해야 한다.
  Content-Length 보다 buffer size가 넘어간다면 넘어간 값부터는 쓰레기 값이라고 판단 할 수 있다.
  client_max_body_size 는 validator에서 판단해야 하나?
*/
void	ws::Request::parse_request_body() {

  /* token 대입 시간 테스트 해야함 */
  std::string::size_type i = _request_body.length();

  // _request_body = token.rdall(buffer);

  for (char c = _buffer.get(); i <= _content_length || i <= _client_max_body_size; c = _buffer.get(), ++i) {
    if (_buffer.eof())
      break;

    _request_body.push_back(c);
  }

  if (i == _client_max_body_size)
    _status = 413;

  if (i == _content_length || i == _client_max_body_size)
    _eof = true;
}

void  ws::Request::parse_request_uri(std::string uri) {
  std::string::size_type mark_pos = uri.find("?");

  if (mark_pos == uri.npos)
    _request_uri = uri;
  else {
    std::stringstream buffer;
    std::string key;
    std::string value;

    _request_uri = uri.substr(0, mark_pos);
    buffer << uri.substr(mark_pos + 1);

    while (!buffer.eof()) {
      key = rdline('=');
      value = rdline('&');
      _request_uri_query.insert(query_type::value_type(key, value));
    }
  }
}

bool ws::Request::parse_request_start_line() {
  std::string::size_type pos1 = _token.find(' ');

  if (pos1 == std::string::npos)
    return false;

  std::string::size_type pos2 = _token.find(' ', pos1 + 1);

  if (pos2 == std::string::npos)
    return false;

  _method = _token.substr(0, pos1);
  parse_request_uri(_token.substr(pos1 + 1, pos2 - pos1 - 1));
  _http_version = _token.substr(pos2 + 1);

  return true;
}

void	ws::Request::parse_request_header() {
  std::string key;
  std::string value;
  std::string::size_type pos;

  while (!(_buffer.eof() || _status)) {
    rd_http_line();

    if (_token.length() < 2 || _token.compare(_token.length() - 2, 2, "\r\n")) {
      _buffer.clear();
      _buffer << _token;
      return;
    }

    if (_token == "\r\n")
        break;

    _token.erase(_token.length() - 2, 2);

    pos = _token.find(":");
    if (pos == std::string::npos) {
      if (!parse_request_start_line())
        _status = BAD_REQUEST;
      continue;
    }

    key = _token.substr(0, pos);
    value = _token.substr(pos + 2, _token.length() - pos - 1);

    header_parse_map_type::iterator header_iter = _header_parser.find(key);

    if (header_iter != _header_parser.end())
      (this->*header_iter->second)(value);
    else
      _request_header.insert(header_type::value_type(key, value));
  }

  if (_token != "\r\n")
    _status = BAD_REQUEST;
  if (_content_length == std::numeric_limits<std::size_t>::max() && !_chunked)
    _eof = true;

  _is_header = false;
}

/*
  repository를 header파싱 후 해줘서 client_max_body_size까지만 받아올 지 생각 해 봐야함
*/
int ws::Request::parse_request_message(const ws::Configure& conf, const std::string& message, ws::Repository& repo) {
  _buffer << message;

  /*
    buffer size 가 0 인 경우 어떻게 처리해야 할까?
    case 1: kernel buffer가 모두 읽힌 뒤 발생한 kevent -> buffer size == 0 으로 들어옴
    case 2: tcp 또는 application layer 오류 등등

    아마 _eof 변수로 파싱을 관리해서 괜찮지 않을까 라는 뇌피셜
  */

  if (_is_header) {
    parse_request_header();

    if (_is_header) // chunked header
      return _status;

    const ws::Server& curr_server = conf.find_server(this->get_listen(), this->get_server_name());
    _client_max_body_size = curr_server.get_client_max_body_size();
    repo(curr_server, *this);
  }

  /* body가 없거나 _status가 양수일 경우 eof 설정 */
  if (_status)
    _eof = true;
  if (_eof)
    return _status;

  if (!_chunked)
    parse_request_body();
  else
    parse_request_chunked_body();

  return _status;
}

void  ws::Request::clear() {
  _eof = false;

  _method.clear();
  _request_uri.clear();
  _request_uri_query.clear();
  _http_version.clear();

  _request_header.clear();
  _request_body.clear();

  _content_length = std::numeric_limits<size_t>::max();
  _server_name.clear();
  _port = 0;
  _connection.clear();
  _transfer_encoding.clear();

  _header_parser.clear();
  _status = 0;
  _chunked = false;
  _chunked_line_type = 0;
  _chunked_byte = 0;
  _client_max_body_size = 0;
  _token.clear();
  _buffer.rdbuf();
}

/* parser function */

void  ws::Request::parse_host(const std::string& value) {
  std::string::size_type pos = value.find_first_of(":");

  if (!_server_name.empty()) {
    _status = BAD_REQUEST;
    return;
  }

  _server_name = value.substr(0, pos);
  if (inet_addr(_server_name.c_str()) != INADDR_NONE) //ip
    _server_name = "_";

  if (pos != std::string::npos)
    _port = ws::Util::stoul((value.substr(pos + 1, value.length())).c_str(), std::numeric_limits<u_int16_t>::max());
}

void  ws::Request::parse_connection(const std::string& value) {
  _connection = value;
}

void  ws::Request::parse_content_length(const std::string& value) {
  std::string::size_type content_length = ws::Util::stoul(value);

  if (content_length == value.npos) {
    _status = BAD_REQUEST;
    return;
  }

  _content_length = content_length;
}

void  ws::Request::parse_content_type(const std::string& value) {
  //text/html; charset=utf-8
  std::string::size_type pos = value.find(';');

  _content_type = value.substr(0, pos);
}

void  ws::Request::parse_transfer_encoding(const std::string& value) {
  _transfer_encoding = value;
  if (value == "chunked")
    _chunked = true;
  return;
}

/* Else private function */

void  ws::Request::insert_require_header_field() {
  _header_parser.insert(header_parse_map_type::value_type("Host", &Request::parse_host));
  _header_parser.insert(header_parse_map_type::value_type("Connection", &Request::parse_connection));
  _header_parser.insert(header_parse_map_type::value_type("Content-Length", &Request::parse_content_length));
  _header_parser.insert(header_parse_map_type::value_type("Content-Type", &Request::parse_content_type));
  _header_parser.insert(header_parse_map_type::value_type("Transfer-Encoding", &Request::parse_transfer_encoding));
}

ws::Token&  ws::Request::rdword() {
  _token.rdword(_buffer);
  return _token;
}

ws::Token& ws::Request::rdline(char delim) {
  _token.rdline(_buffer, delim);
  return _token;
}

ws::Token& ws::Request::rd_http_line() {
  _token.rd_http_line(_buffer);
  return _token;
}

ws::Token& ws::Request::rdall() {
  _token.rdall(_buffer);
  return _token;
}

/* getter */

/*
  _eof
  case false: In progress request message parsing
  case  true: Request message parsing is done
*/
bool ws::Request::eof() const throw() {
  return _eof;
}

const std::string& ws::Request::get_method() const throw() {
	return _method;
}

const std::string& ws::Request::get_uri() const throw() {
	return _request_uri;
}

const ws::Request::query_type& ws::Request::get_uri_query() const throw() {
	return _request_uri_query;
}

const std::string& ws::Request::get_version() const throw() {
	return _http_version;
}

const ws::Request::header_type& ws::Request::get_request_header() const throw() {
	return _request_header;
}

const std::string& ws::Request::get_request_body() const throw() {
	return _request_body;
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

const std::string& ws::Request::get_connection() const throw() {
  return _connection;
}

const std::string& ws::Request::get_transfer_encoding() const throw() {
  return _transfer_encoding;
}

//todo: print test
void ws::Request::test() {
  std::cout << "host: _: " << _listen.first << ", port: " << _listen.second << std::endl;
  std::cout << "eof: " << _eof << std::endl;
  std::cout << "method: " << _method << std::endl;
  std::cout << "request_uri: " << _request_uri << std::endl;
  std::cout << "request_uri_query:" << std::endl;
  for (query_type::iterator it = _request_uri_query.begin(); it != _request_uri_query.end(); ++it) {
    std::cout << "  Key: " << it->first << ", Value: " << it->second << std::endl;
  }
  std::cout << "http version: " << _http_version << std::endl;
  std::cout << "content length: " << _content_length << std::endl;
  std::cout << "content type: " << _content_type << std::endl;
  std::cout << "server name: " << _server_name << std::endl;
  std::cout << "run_server: " << _connection << std::endl;
  std::cout << "transfer encoding: " << _transfer_encoding << std::endl;

  
  std::cout << "_status: " << _status << std::endl;
  std::cout << "_chunked: " << (_chunked ? "true" : "false") << std::endl;
  std::cout << "_chunked_line_type: " << _chunked_line_type << std::endl;
  std::cout << "_chunked_byte: " << _chunked_byte << std::endl;
  std::cout << "_client_max_body_size: " << _client_max_body_size << std::endl;
  
  std::cout << "\nHeader:" << std::endl;
  for (header_type::iterator it = _request_header.begin(); it != _request_header.end(); ++it)
    std::cout << RED << "> " << NC << it->first << " " << it->second << std::endl;
  std::cout << "\nBody:" << std::endl;
  std::cout << _request_body << std::endl;
}
