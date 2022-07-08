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
    if (client_data.repository->set_status(0))
      break;
  }
}

void ws::Validator::check_method(client_value_type& client_data) {
  std::string method = client_data.request->get_method();  
  ws::Location::limit_except_vec_type limit_except_vec = client_data.repository->get_location()->get_limit_except_vec();

  for (ws::Location::limit_except_vec_type::iterator it = limit_except_vec.begin(); it != limit_except_vec.end(); ++it) {
    if (*it == method)
      return;
  }
  client_data.repository->set_status(FORBIDDEN);
  // 405 error : 메소드는 허용되었지만 실패
}

void ws::Validator::check_uri(client_value_type& client_data) {
  
  if (client_data.repository->get_location() == NULL) {
    client_data.repository->set_status(NOT_FOUND);
    return;
  }

  struct stat buf;
  if (lstat(client_data.request->get_uri().c_str(), &buf) > 0) {
    client_data.repository->set_status(NOT_FOUND);
    return;
  }
  if (S_ISDIR(buf.st_mode) && client_data.repository->get_server()->get_autoindex() == false) {
    client_data.repository->set_status(NOT_FOUND);
    return;
  }
}

void ws::Validator::check_version(client_value_type& client_data) {
  if (client_data.request->get_version() != "HTTP/1.1")
    client_data.repository->set_status(HTTP_VERSION_NOT_SUPPORTED);
  return;
}

void ws::Validator::check_content_length(ws::Validator::client_value_type& client_data) {
  if (
    (
      client_data.request->get_content_length() == std::numeric_limits<unsigned long>::max()
      && (client_data.request->get_transfer_encoding().empty())
    )
    || (
      client_data.request->get_content_length() != std::numeric_limits<unsigned long>::max()
      && !(client_data.request->get_transfer_encoding().empty())
    )
  )
    client_data.repository->set_status(BAD_REQUEST);

  if (client_data.request->get_content_length() != client_data.request->get_request_body().length())
    client_data.repository->set_status(BAD_REQUEST);

  return;
}

void ws::Validator::check_connection(ws::Validator::client_value_type& client_data) {
  const std::string connection = client_data.request->get_connection();

  if (!(connection == "close" || connection == "keep-alive"))
    client_data.repository->set_status(BAD_REQUEST);
}

void ws::Validator::check_transfer_encoding(ws::Validator::client_value_type& client_data) {
  (void) client_data; // todo
}

void ws::Validator::check_host(ws::Validator::client_value_type& client_data) {
  std::string host = client_data.request->get_server_name();
  if (host == "")
    client_data.repository->set_status(BAD_REQUEST);
}

// if (repository->empty())
//   _status = ws::Code::NOT_FOUND;
