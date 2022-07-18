#include "RequestParser.hpp"

ws::RequestParser::RequestParser(): _conf(NULL), _fatal(NULL), _status(NULL), _request(NULL), _repository(NULL) {
  _header_parser.insert(header_parse_map_type::value_type("Host", &RequestParser::parse_host));
  _header_parser.insert(header_parse_map_type::value_type("Connection", &RequestParser::parse_connection));
  _header_parser.insert(header_parse_map_type::value_type("Content-Length", &RequestParser::parse_content_length));
  _header_parser.insert(header_parse_map_type::value_type("Content-Type", &RequestParser::parse_content_type));
  _header_parser.insert(header_parse_map_type::value_type("Transfer-Encoding", &RequestParser::parse_transfer_encoding));
}

ws::RequestParser::~RequestParser() {}

void  ws::RequestParser::init_conf(const ws::Configure& conf) {
  _conf = &conf;
}

void  ws::RequestParser::init_request_parser(client_value_type& client_data, const char* message, const int read_size) {
  _fatal = &client_data.fatal;
  _status = &client_data.status;
  _request = &client_data.request;
  _repository = &client_data.repository;

  _read_size = read_size;
  _message = message;
  _buffer = &_request->r_buffer();
  _buffer->write(message, read_size);

  _token.clear();

  // todo critical fatal fix 기존 버퍼가 남아있는 경우 request_message_offset의 값을 남은 버퍼 사이즈만큼 빼줘야 한다.
  _request->set_request_message_offset(0);
}

void  ws::RequestParser::end_request_parser() {
  _fatal = NULL;
  _status = NULL;
  _request = NULL;
  _repository = NULL;

  _read_size = 0;
  _message = NULL;
  _buffer = NULL;

  _token.clear();
}

void  ws::RequestParser::parse_request_message(client_value_type& client_data, const char* message, const int read_size) {
  init_request_parser(client_data, message, read_size);

  if (_request->get_is_header()) {
    parse_request_header();

    if (_request->get_is_header()) {
      end_request_parser();
      return;
    }

    const ws::Server& curr_server = _conf->find_server(_request->get_listen(), _request->get_server_name());
    _request->set_client_max_body_size(curr_server.get_client_max_body_size());
    (*_repository)(curr_server, *_request);

    // todo why and??
    if (_request->get_content_length() != std::string::npos && _request->get_content_length() > _request->get_client_max_body_size())
      *_status = 413;
  }

  if (*_status > 0)
    _request->set_eof(true);
  if (_request->get_eof()) {
    end_request_parser();
    return;
  }

  if (!_request->get_chunked())
     parse_request_body();
  else
     parse_request_chunked_body();

  end_request_parser();
}

/* ======================================= */
/*         Request message parser          */
/* ======================================= */

void  ws::RequestParser::parse_request_header() {
  std::string key;
  std::string value;
  size_t      pos;

  while (!(_buffer->eof() || *_status > 0)) {
    rd_http_line();

    _request->add_request_message_offset(_token.length());

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
        *_status = BAD_REQUEST;
      continue;
    }

    key = _token.substr(0, pos);
    value = _token.substr(pos + 2, _token.length() - pos - 1);

    header_parse_map_type::iterator header_iter = _header_parser.find(key);

    if (header_iter != _header_parser.end())
      (this->*header_iter->second)(value);
    else
      _request->set_request_header(header_map_type::value_type(key, value));
  }

  if (_token != "\r\n")
    *_status = BAD_REQUEST;
  if (_request->get_content_length() == std::numeric_limits<std::size_t>::max() && !_request->get_chunked())
    _request->set_eof(true);

  _request->set_is_header(false);
}

void  ws::RequestParser::parse_request_body() {
  const size_t body_max_size = _request->get_client_max_body_size();
  const size_t content_length = _request->get_content_length();
  const size_t curr_body_size = _request->get_request_body_size();
  const size_t message_offset = _request->get_request_message_offset();
  size_t message_size = _read_size - message_offset;

  if (curr_body_size + message_size > body_max_size) {
    *_status = 413;
    _request->set_eof(true);
    return;
  }

  if (curr_body_size + message_size > content_length)
    message_size = content_length - curr_body_size;

  _request->set_request_body(_message, message_offset, message_size);
  _request->add_request_message_offset(message_size);
}

void  ws::RequestParser::parse_request_chunked_body() {
  bool chunked_line_type = _request->get_chunked_line_type();
  bool chunked_eof = _request->get_chunked_eof();

  while (*_status == 0 && !(_token == "0" && chunked_line_type) && !chunked_eof) {

    // todo char* offset 설정으로 하면 더 빠를듯?
    rd_http_line();

    if (_token.length() < 2 || _token.compare(_token.length() - 2, 2, "\r\n")) {
      _buffer->clear();
      (*_buffer) << _token;

      _request->set_chunked_line_type(chunked_line_type);
      _request->set_chunked_eof(chunked_eof);

      return;
    }

    _token.erase(_token.length() - 2, 2);

    if (!chunked_line_type) {
      _request->add_request_message_offset(_token.length() + 2);
      _request->set_chunked_byte(ws::Util::stoul(_token, std::numeric_limits<unsigned long>::max(), 0, "0123456789ABCDEF"));
      _request->set_chunked_line_type(true);
      if (_request->get_chunked_byte() == std::string::npos) {
        *_status = BAD_REQUEST;
        return;
      }
    }
    else {
      _request->add_chunked_byte(static_cast<int>(_token.length()) * -1);
      _request->set_request_body(_message, _request->get_request_message_offset(), _token.length());
      if (_request->get_chunked_byte() == 0)
        _request->set_chunked_line_type(false);
      _request->add_request_message_offset(_token.length());
    }
  }

  rd_http_line();

  if (*_status > 0) {
    _request->set_eof(true);
    return;
  }

  if (_token != "\r\n") {
    _request->set_chunked_eof(true);
    _buffer->clear();
    _buffer->write(_message + _request->get_request_message_offset(), static_cast<std::streamsize>(_token.length()));
    return;
  }

  _request->set_eof(true);
}

/* ======================================= */
/*        Header start line parser         */
/* ======================================= */

void  ws::RequestParser::parse_request_uri(const std::string& uri) {
  std::string::size_type mark_pos = uri.find("?");

  if (mark_pos == uri.npos)
    _request->set_request_uri(uri);
  else {
    std::stringstream buffer;
    std::string key;
    std::string value;

    _request->set_request_uri(uri.substr(0, mark_pos));
    buffer << uri.substr(mark_pos + 1);

    while (!buffer.eof()) {
      key = rdline('=');
      value = rdline('&');
      _request->add_request_message_offset(key.length());
      _request->add_request_message_offset(value.length());
      _request->set_request_uri_query(query_map_type::value_type(key, value));
    }
  }
}

bool  ws::RequestParser::parse_request_start_line() {
  std::string::size_type pos1 = _token.find(' ');

  if (pos1 == std::string::npos)
    return false;

  std::string::size_type pos2 = _token.find(' ', pos1 + 1);

  if (pos2 == std::string::npos)
    return false;

  _request->set_method(_token.substr(0, pos1));
  parse_request_uri(_token.substr(pos1 + 1, pos2 - pos1 - 1));
  _request->set_http_version(_token.substr(pos2 + 1));

  return true;
}

/* ======================================= */
/*              Header parser              */
/* ======================================= */

void  ws::RequestParser::parse_host(const std::string& value) {
  std::string::size_type pos = value.find_first_of(":");

  if (!_request->get_server_name().empty()) {
    *_status = BAD_REQUEST;
    return;
  }

  _request->set_server_name(value.substr(0, pos));
  if (inet_addr(_request->get_server_name().c_str()) != INADDR_NONE) //ip
    _request->set_server_name("_");

  if (pos != std::string::npos)
    _request->set_port(ws::Util::stoul((value.substr(pos + 1, value.length())).c_str(), std::numeric_limits<u_int16_t>::max()));
}

void  ws::RequestParser::parse_connection(const std::string& value) {
  _request->set_connection(value);
}

void  ws::RequestParser::parse_content_length(const std::string& value) {
  std::string::size_type content_length = ws::Util::stoul(value);

  if (content_length == value.npos) {
    *_status = BAD_REQUEST;
    return;
  }

  _request->set_content_length(content_length);
}

void  ws::RequestParser::parse_content_type(const std::string& value) {
  //text/html; charset=utf-8
  std::string::size_type pos = value.find(';');

  _request->set_content_type(value.substr(0, pos));
}

void  ws::RequestParser::parse_transfer_encoding(const std::string& value) {
  _request->set_transfer_encoding(value);
  if (value == "chunked")
    _request->set_chunked(true);
}

/* ======================================= */
/*             Buffer tokenizer            */
/* ======================================= */

ws::Token& ws::RequestParser::rdline(char delim) {
  _token.rdline(*_buffer, delim);
  return _token;
}

ws::Token& ws::RequestParser::rd_http_line() {
  _token.rd_http_line(*_buffer);
  return _token;
}