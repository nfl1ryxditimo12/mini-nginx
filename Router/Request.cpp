#include "Request.hpp"

#include "Repository.hpp"

ws::Request::Request(const ws::Configure::listen_type& listen)
  : _listen(listen), _eof(false), _content_length(std::numeric_limits<std::size_t>::max()), _port(), _session_id(UINT_MAX),
    _status(0), _chunked(false), _chunked_line_type(false), _chunked_eof(false), _chunked_byte(std::string::npos), _client_max_body_size(0), _is_header(true),
    _token() {
  insert_require_header_field();
}

ws::Request::Request(const Request& cls) {
  _listen = cls._listen;
  _eof = cls._eof;

  _method = cls._method;
  _request_uri = cls._request_uri;
  _http_version = cls._http_version;

  _request_header = cls._request_header;
  _request_body = cls._request_body;

  _content_length = cls._content_length;
  _content_type = cls._content_type;
  _server_name = cls._server_name;
  _port = cls._port;
  _session_id = cls._session_id;
  _connection = cls._connection;
  _transfer_encoding = cls._transfer_encoding;
  _session_id = cls._session_id;
  _name = cls._name;
  _secret_key = cls._secret_key;

  _header_parser = cls._header_parser;
  _status = cls._status;
  _chunked = cls._chunked;
  _chunked_line_type = cls._chunked_line_type;
  _chunked_eof = cls._chunked_eof;
  _chunked_byte = cls._chunked_byte;
  _client_max_body_size = cls._client_max_body_size;
  _is_header = cls._is_header;
}

ws::Request::~Request() {}

void	ws::Request::parse_request_chunked_body() {
  while (!_status && !(_token == "0" && _chunked_line_type) && !_chunked_eof) {
    rd_http_line();

    if (_token.length() < 2 || _token.compare(_token.length() - 2, 2, "\r\n")) {
      _buffer->clear();
      *_buffer << _token;
      return;
    }

    _token.erase(_token.length() - 2, 2);

    if (!_chunked_line_type) {
      _chunked_byte = ws::Util::stoul(_token, std::numeric_limits<unsigned long>::max(), 0, "0123456789ABCDEF");
      _chunked_line_type = true;

      if (_chunked_byte == std::string::npos)
        _status = BAD_REQUEST;
    }
    else {
      _chunked_byte -= _token.length();
      _request_body += _token;
      if (_chunked_byte == 0)
          _chunked_line_type = 0;
    }
  }

  rd_http_line();

  if (_status > 0) {
    _eof = true;
    return;
  }

  if (_token != "\r\n") {
    _chunked_eof = true;
    _buffer->clear();
    *_buffer << _token;
    return;
  }
  _eof = true;
  if (_request_body.length() > _client_max_body_size)
    _status = PAYLOAD_TOO_LARGE;
}

void	ws::Request::parse_request_body() {
  std::string::size_type i = _request_body.length();

  for (; i < _content_length; ++i)
    _request_body.push_back(static_cast<char>(_buffer->get()));

  if (i == _content_length)
    _eof = true;
}

void  ws::Request::parse_request_uri(const std::string& uri) {
  std::string::size_type mark_pos = uri.find('?');

  _request_uri = uri.substr(0, mark_pos);
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
  _http_version = _token.substr(pos2 + 1); // todo?

  return true;
}

void	ws::Request::parse_request_header() {
  std::string key;
  std::string value;
  std::string::size_type pos;

  while (!_status) {
    rd_http_line();

    if (_token.length() < 2 || _token.compare(_token.length() - 2, 2, "\r\n")) {
      _buffer->clear();
      *_buffer << _token;
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

int ws::Request::parse_request_message(const ws::Configure& conf, ws::Buffer* buffer, ws::Repository& repo) {
  _buffer = buffer;

  if (_is_header) {
    parse_request_header();

    if (_is_header) // chunked header
      return _status;

    const ws::Server& curr_server = conf.find_server(this->get_listen(), this->get_server_name());
    repo(curr_server, *this);
    _client_max_body_size = repo.get_client_max_body_size();
    if (_content_length != std::string::npos && _content_length > _client_max_body_size)
      _status = PAYLOAD_TOO_LARGE;
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
  _chunked_line_type = false;
  _chunked_byte = 0;
  _client_max_body_size = 0;
  _token.clear();
  _buffer->clear();
}

/* parser function */

void  ws::Request::parse_host(const std::string& value) {
  std::string::size_type pos = value.find_first_of(':');

  if (!_server_name.empty()) {
    _status = BAD_REQUEST;
    return;
  }

  _server_name = value.substr(0, pos);
  if (inet_addr(_server_name.c_str()) != INADDR_NONE) //ip
    _server_name = "_";

  if (pos != std::string::npos)
    _port = ws::Util::stoul((value.substr(pos + 1, value.length())), std::numeric_limits<u_int16_t>::max());
}

void  ws::Request::parse_connection(const std::string& value) {
  _connection = value;
}

void  ws::Request::parse_content_length(const std::string& value) {
  std::string::size_type content_length = ws::Util::stoul(value);

  if (content_length == std::string::npos) {
    _status = BAD_REQUEST;
    return;
  }

  _content_length = content_length;
}

void  ws::Request::parse_content_type(const std::string& value) {
  std::string::size_type pos = value.find(';');

  _content_type = value.substr(0, pos);
}

void  ws::Request::parse_transfer_encoding(const std::string& value) {
  _transfer_encoding = value;
  if (value == "chunked")
    _chunked = true;
}

void  ws::Request::parse_session_id(const std::string &value) {
  std::string::size_type pos = value.find('=');
  std::string id = value.substr(pos + 1, value.length());
  if (pos == std::string::npos || id == "0")
    _session_id = std::numeric_limits<unsigned int>::max();
  else
    _session_id = ws::Util::stoul(id, std::numeric_limits<unsigned int>::max());
}

void  ws::Request::parse_name(const std::string &value) {
  _name = value;
}

void ws::Request::parse_secret_key(const std::string &value) {
  _secret_key = value;
}

/* Else private function */

void  ws::Request::insert_require_header_field() {
  _header_parser.insert(header_parse_map_type::value_type("Host", &Request::parse_host));
  _header_parser.insert(header_parse_map_type::value_type("Connection", &Request::parse_connection));
  _header_parser.insert(header_parse_map_type::value_type("Content-Length", &Request::parse_content_length));
  _header_parser.insert(header_parse_map_type::value_type("Content-Type", &Request::parse_content_type));
  _header_parser.insert(header_parse_map_type::value_type("Transfer-Encoding", &Request::parse_transfer_encoding));
  _header_parser.insert(header_parse_map_type::value_type("Cookie", &Request::parse_session_id));
  _header_parser.insert(header_parse_map_type::value_type("Name", &Request::parse_name));
  _header_parser.insert(header_parse_map_type::value_type("Secret-Key", &Request::parse_secret_key));

}

ws::Token&  ws::Request::rdword() {
  _token.rdword(*_buffer);
  return _token;
}

ws::Token& ws::Request::rdline(char delim) {
  _token.rdline(*_buffer, delim);
  return _token;
}

ws::Token& ws::Request::rd_http_line() {
  _token.rd_http_line(*_buffer);
  return _token;
}

ws::Token& ws::Request::rdall() {
  _token.rdall(*_buffer);
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

const unsigned int& ws::Request::get_session_id() const throw() {
  return _session_id;
}

const std::string& ws::Request::get_name() const throw() {
  return _name;
}

const std::string& ws::Request::get_secret_key() const throw() {
  return _secret_key;
}

void ws::Request::set_session_id(unsigned int session_id) throw() {
  _session_id = session_id;
}
