#include "Validator.hpp"
#include "Socket.hpp"
#include <sys/stat.h>
// #include <iostream>//@

extern bool webserv_fatal;

ws::Validator::Validator() throw() : _session(NULL) {
  try {
    _check_func_vec.push_back(&Validator::init_content_type_parser);
    _check_func_vec.push_back(&Validator::check_method);
    _check_func_vec.push_back(&Validator::check_uri);
    _check_func_vec.push_back(&Validator::check_version);
    _check_func_vec.push_back(&Validator::check_host);
    _check_func_vec.push_back(&Validator::check_connection);
    _check_func_vec.push_back(&Validator::check_content_length);
    _check_func_vec.push_back(&Validator::check_content_type);
    _check_func_vec.push_back(&Validator::check_transfer_encoding);
    _check_session_func_vec.push_back(&Validator::check_session_id);
    _check_session_func_vec.push_back(&Validator::check_secret_key);
  } catch (...) {
    webserv_fatal = true;
  }
}

ws::Validator::~Validator() {}

void ws::Validator::operator()(const session_map_type& session, client_value_type& client_data, bool is_session) {
  _session = &session;

  for (check_func_vec::iterator it = _check_func_vec.begin(); it != _check_func_vec.end(); ++it) {
    (this->**it)(client_data);
    if (client_data.status != 0) //todo: < 300
      break;
  }
  if (is_session) {
    for (check_session_func_vec::iterator it = _check_session_func_vec.begin(); it != _check_session_func_vec.end(); ++it) {
      (this->**it)(client_data);
      if (client_data.status != 0)
        break;
    }
  }
}

void ws::Validator::init_content_type_parser(client_value_type&) {
  _content_type_parser.insert("text/plain");
  _content_type_parser.insert("text/html");
  _content_type_parser.insert("image/jpeg");
  _content_type_parser.insert("image/png");
  _content_type_parser.insert("application/octet-stream");
}

void ws::Validator::check_method(client_value_type& client_data) {
  const std::string& request_method = client_data.request.get_method();
  const limit_except_vec_type& limit_except_vec = client_data.repository.get_location()->get_limit_except_vec();

  if (
    !(request_method == "GET" ||
    request_method == "HEAD" ||
    request_method == "POST" ||
    request_method == "DELETE" ||
    request_method == "PUT")
  )
    client_data.status = METHOD_NOT_ALLOWED;

  for (
    limit_except_vec_type::const_iterator limit_except = limit_except_vec.begin();
    limit_except != limit_except_vec.end();
    ++limit_except
  ) {
    if (request_method == *limit_except)
      return;
  }
  client_data.status = METHOD_NOT_ALLOWED;
}

void ws::Validator::check_uri(client_value_type& client_data) {
  const struct stat& file_status = client_data.repository.get_file_stat();
  const std::string& method = client_data.repository.get_method();

  if (client_data.request.get_uri() == "/session")
    return;

  if (S_ISREG(file_status.st_mode)) {
      // client_data.status = NOT_MODIFIED; -> file download
    if ((file_status.st_mode & S_IREAD) == 0)
      client_data.status = FORBIDDEN;
  } else if (!(S_ISDIR(file_status.st_mode)) && method != "POST" && method != "PUT" && method != "DELETE")
    client_data.status = NOT_FOUND;
}

void ws::Validator::check_version(client_value_type& client_data) {
  if (client_data.request.get_version() != "HTTP/1.1")
    client_data.status = HTTP_VERSION_NOT_SUPPORTED;
}

void ws::Validator::check_host(client_value_type& client_data) {
  const std::string& host = client_data.request.get_server_name();

  if (host == "")
    client_data.status = BAD_REQUEST;

  //host가 잘못된 값 = 400 -> 공백같은거...?!
}

void ws::Validator::check_connection(client_value_type& client_data) {
  const std::string& connection = client_data.request.get_connection();

  if (!(connection == "close" || connection == "keep-alive" || connection == ""))
    client_data.status = BAD_REQUEST;
}

void ws::Validator::check_content_length(client_value_type& client_data) {
  const ws::Request* const request = &client_data.request;

  if (request->get_content_length() == std::numeric_limits<unsigned long>::max())
    return;

  client_data.status = 0;

  if (!(request->get_transfer_encoding().empty()))
    client_data.status = BAD_REQUEST;

  if (request->get_method() == "POST") {
    if (request->get_content_length() != request->get_request_body().length())
      client_data.status = BAD_REQUEST;
  } else {
    if (request->get_request_body().empty())
      client_data.status = BAD_REQUEST;
    else {
      if (request->get_content_length() != request->get_request_body().length())
        client_data.status = BAD_REQUEST;
    }
  }
}

void ws::Validator::check_content_type(client_value_type &client_data) {
  header_type request_header = client_data.request.get_request_header();
  header_type::const_iterator header_iter = request_header.find("Content-Type");

  if (header_iter == request_header.end())
    return;

  const std::string& content_type = header_iter->second;

  if (_content_type_parser.find(content_type) == _content_type_parser.end())
    client_data.status = UNSUPPORTED_MEDIA_TYPE;
}

void ws::Validator::check_transfer_encoding(client_value_type& client_data) {
  const std::string& transfer_encoding = client_data.request.get_transfer_encoding();

  if (!(transfer_encoding == "chunked" || transfer_encoding == ""))
    client_data.status = BAD_REQUEST;
}

  // todo?: make Util::eraseWS
void ws::Validator::check_session_id(client_value_type &client_data) {
  const std::string& method = client_data.request.get_method();
  const unsigned int& session_id = client_data.request.get_session_id();

  if (method == "GET" || method == "DELETE") {
    if (_session->find(session_id) == _session->end())
      client_data.status = UNAUTHORIZED;
  }
}

void ws::Validator::check_secret_key(client_value_type &client_data) {
  const std::string& method = client_data.request.get_method();
  const std::string& secret_key = client_data.request.get_secret_key();

  if (method == "POST") {
    if (secret_key != "hellowebserv")
      client_data.status = UNAUTHORIZED;
  }
}

//connection close인데 close로 설정되어있지 않으면 error <- response에서 status랑 비교해서 결정하기
