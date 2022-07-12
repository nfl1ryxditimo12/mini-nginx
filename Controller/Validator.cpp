#include "Validator.hpp"
#include "Socket.hpp"
#include <sys/stat.h>

ws::Validator::Validator() {
  _check_func_vec.push_back(&Validator::check_method);
  _check_func_vec.push_back(&Validator::check_uri);
  _check_func_vec.push_back(&Validator::check_version);
  _check_func_vec.push_back(&Validator::check_content_length);
  _check_func_vec.push_back(&Validator::check_connection);
  _check_func_vec.push_back(&Validator::check_transfer_encoding);
  _check_func_vec.push_back(&Validator::check_host);
}

ws::Validator::~Validator() {}

void ws::Validator::operator()(client_value_type& client_data) {
  for (check_func_vec::iterator it = _check_func_vec.begin(); it != _check_func_vec.end(); ++it) {
    (this->**it)(client_data);
    if (client_data.status != 0)
      break;
  }
}

void ws::Validator::check_method(client_value_type& client_data) {
  std::string request_method = client_data.request.get_method();
  limit_except_vec_type limit_except_vec = client_data.repository.get_location()->get_limit_except_vec();

  for (
    limit_except_vec_type::iterator limit_except = limit_except_vec.begin();
    limit_except != limit_except_vec.end();
    ++limit_except
    ) {
    if (request_method == *limit_except)
      return;
    if (request_method == "HEAD")
      if (*limit_except == "GET")
        return;
  }
  client_data.status = FORBIDDEN;
  // client_data.status = METHOD_NOT_ALLOWED; //todo
}

void ws::Validator::check_uri(client_value_type& client_data) {
  const ws::Repository& repository = client_data.repository;
  const std::string& uri = client_data.request.get_uri();
  std::string url = repository.get_root() + uri;
  struct stat file_status;

  if (lstat(url.c_str(), &file_status) != 0) {
    client_data.status = NOT_FOUND;
  } else {
    if (S_ISREG(file_status.st_mode)) {
      // if (not html file) { //@
      //   client_data.status = NOT_MODIFIED;
      //   return;
      // }
      client_data.status = OK;
      if (file_status.st_mode & S_IREAD) //@읽기권한 없으면 403
        client_data.status = FORBIDDEN;
    } else if (S_ISDIR(file_status.st_mode)) {
      if (url[url.length() - 1] == '/')
        url += '/';
      client_data.status = OK; //default file 권한은 url넘겨준 후에 repository에서 검사
    } else {
      if (repository.get_server()->get_autoindex() == false) {
        client_data.status = NOT_FOUND;
        return;
      }
      // autoindex가 실행되는 경우는 repository에서 결정
    }
  }
}

void ws::Validator::check_version(client_value_type& client_data) {
  if (client_data.request.get_version() != "HTTP/1.1")
    client_data.status = HTTP_VERSION_NOT_SUPPORTED;
  return;
}

void ws::Validator::check_content_length(ws::Validator::client_value_type& client_data) {
  const ws::Request* const request = &client_data.request;

  if (
    (
      request->get_content_length() == std::numeric_limits<unsigned long>::max()
      && (request->get_transfer_encoding().empty())
      && (request->get_method() == "POST")
    )
    || (
      request->get_content_length() != std::numeric_limits<unsigned long>::max()
      && !(request->get_transfer_encoding().empty())
    )
  )
    client_data.status = BAD_REQUEST;

  if (
    (request->get_content_length() != request->get_request_body().length())
    && (request->get_method() == "POST")
  )
    client_data.status = BAD_REQUEST;

  return;
}

void ws::Validator::check_connection(ws::Validator::client_value_type& client_data) {
  const std::string connection = client_data.request.get_connection();

  if (!(connection == "close" || connection == "keep-alive" || "")) // todo
    client_data.status = BAD_REQUEST;
}

void ws::Validator::check_transfer_encoding(ws::Validator::client_value_type& client_data) {
  (void) client_data; // todo
}

void ws::Validator::check_host(ws::Validator::client_value_type& client_data) {
  std::string host = client_data.request.get_server_name();
  if (host == "")
    client_data.status = BAD_REQUEST;
}

// if (repository->empty())
//   _status = ws::Code::NOT_FOUND;
