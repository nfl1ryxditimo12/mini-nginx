#include "Request.hpp"
#include "Repository.hpp"

ws::Request::Request(const ws::Configure::listen_type& listen): _listen(listen) {
  insert_require_header_field();

  _eof = false;

  _content_length = 0;

  _status = 0;
  _chunked_byte = 0;
  _chunked_line_type = 0;
  _chunked = false;
  _client_max_body_size = 0;
}

ws::Request::~Request() {}

/*
  chunked인 경우 어떻게 처리할까?
  chunk-start-line 파싱 -> chunked-content push_back 형식?
  만약 chunked-content-length보다 chunked-content가 적게 들어오면?
*/
void	ws::Request::parse_request_chunked_body(ws::Token& token, std::stringstream& buffer) {

  while (!buffer.eof() && !_status) {
    token.rd_http_line(buffer);
    if (!_chunked_line_type) {
      _chunked_byte = ws::hextoul(token);
      _chunked_line_type = 1;
      if (_chunked_byte == std::string::npos)
        _status = BAD_REQUEST;
    }
    else {
      if (_chunked_byte == 0 && token.length() == 0) {
        _eof = true;
        break;
      }
      for (std::string::size_type i = 0; i < token.length(); ++i, --_chunked_byte) {
        if (_request_body.length() == _client_max_body_size) {
          _eof = true;
          break;
        }
        _request_body.push_back(token[i]);
      }
      if (_chunked_byte < 0)
        _status = BAD_REQUEST;
      else if (_chunked_byte == 0)
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
void	ws::Request::parse_request_body(ws::Token& token, std::stringstream& buffer) {

  /* token 대입 시간 테스트 해야함 */
  std::string::size_type i = _request_body.length();

  _request_body = token.rdall(buffer);

  for (char c = buffer.get(); i <= _content_length && i <= _client_max_body_size; c = buffer.get(), ++i) {
    if (buffer.eof())
      break;

    _request_body.push_back(c);
  }

  if (i == _content_length || i == _client_max_body_size)
    _eof = true;
}

void  ws::Request::parse_request_uri(ws::Token& token, std::string uri) {
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
      key = token.rdline(buffer, '=');
      value = token.rdline(buffer, '&');
      _request_uri_query.insert(query_type::value_type(key, value));
    }
  }
}

void	ws::Request::parse_request_header(ws::Token& token, std::stringstream& buffer) {
  std::string key;
  std::string value;

  /* request start line */
	_method = token.rdword(buffer);
	parse_request_uri(token, token.rdword(buffer));
	_http_version = token.rd_http_line(buffer);
  
  /* request header */
  for (token.rdword(buffer); !buffer.eof() && !_status; token.rdword(buffer)) {
    if (token == "\r\n")
      break;
    if (token.find(":") == token.npos || token[token.length() - 1] != ':') {
      _status = BAD_REQUEST;
      return;
    }
    key = token;
    value = token.rd_http_line(buffer).substr(1, token.length() - 1);

    header_parse_map_type::iterator header_iter;

    if ((header_iter = _header_parser.find(key)) != _header_parser.end())
      (this->*header_iter->second)(value);
    else
      /*
        위에서 변수로 담지만 모든 헤더 필드가 필요할까?
        필요하다면 밑처럼 담아야할듯
      */
      _request_header.insert(header_type::value_type(key, value));
  }

  if (token != "\r\n")
    _status = BAD_REQUEST;
  if (!_content_length && !_chunked)
    _eof = true;
}

/*
  repository를 header파싱 후 해줘서 client_max_body_size까지만 받아올 지 생각 해 봐야함
*/
int ws::Request::parse_request_message(const ws::Configure* conf, ws::Repository* repository, const char* message) {

  ws::Token token;
  std::stringstream buffer;

  buffer << message;

  /*
    buffer size 가 0 인 경우 어떻게 처리해야 할까?
    case 1: kernel buffer가 모두 읽힌 뒤 발생한 kevent -> buffer size == 0 으로 들어옴
    case 2: tcp 또는 application layer 오류 등등

    아마 _eof 변수로 파싱을 관리해서 괜찮지 않을까 라는 뇌피셜
  */

  if (!_request_header.size()) {
    parse_request_header(token, buffer);
    const ws::Server* curr_server = conf->find_server(this->get_listen(), this->get_server_name());
    (*repository)(curr_server, this);
    _client_max_body_size = repository->get_client_max_body_size();
  }

  /* body가 없거나 _status가 양수일 경우 eof 설정 */
  if (_status)
    _eof = true;
  if (_eof)
    return _status;

  if (!_chunked)
    parse_request_body(token, buffer);
  else
    parse_request_chunked_body(token, buffer);

  /*
    default == 0
    HTTP message에 오류가 있는 경우 양수 반환
  */
  return _status;
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

const query_type& ws::Request::get_uri_query() const throw() {
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

const std::string& ws::Request::get_server_name() const throw() {
  return _server_name;
}

const std::string& ws::Request::get_connection() const throw() {
  return _connection;
}

const std::string& ws::Request::get_transfer_encoding() const throw() {
  return _transfer_encoding;
}

/* parser function */

void  ws::Request::parse_host(const std::string& value) {
  std::string server_name = value.substr(0, value.find_first_of(":"));

  if (inet_addr(server_name.c_str()) == static_cast<in_addr_t>(-1))
    _server_name = "_";
  else
    _server_name = server_name;
}

void  ws::Request::parse_connection(const std::string& value) {
  _connection = value;
}

void  ws::Request::parse_content_length(const std::string& value) {
  std::string::size_type content_length = ws::stoul(value);

  if (content_length == value.npos) {
    _status = BAD_REQUEST;
    return;
  }

  _content_length = content_length;
}

void  ws::Request::parse_transfer_encoding(const std::string& value) {
  _transfer_encoding = value;
}

/* Else private function */

void  ws::Request::insert_require_header_field() {
  _header_parser.insert(header_parse_map_type::value_type("Host:", &Request::parse_host));
  _header_parser.insert(header_parse_map_type::value_type("Connection:", &Request::parse_connection));
  _header_parser.insert(header_parse_map_type::value_type("Content-Length:", &Request::parse_content_length));
  _header_parser.insert(header_parse_map_type::value_type("Transfer-Encoding:", &Request::parse_transfer_encoding));
}
