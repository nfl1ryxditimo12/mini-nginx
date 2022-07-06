#include "Validator.hpp"
#include "Socket.hpp"

ws::Validator::Validator() {
  _check_func.push_back(&Validator::check_method);
  _check_func.push_back(&Validator::check_uri);
  _check_func.push_back(&Validator::check_version);
  _check_func.push_back(&Validator::check_content_length);
  _check_func.push_back(&Validator::check_connection);
  _check_func.push_back(&Validator::check_transfer_encoding);
  _check_func.push_back(&Validator::check_host);
  //pushback
}

ws::Validator::~Validator() {}

void ws::Validator::operator()(client_value_type& client_data) {
  for (std::vector<check_func>::iterator it = _check_func.begin(); it != _check_func.end(); ++it) {
    (this->**it)(client_data);
    if (client_data.status > 0)
      break;
  }
}

void ws::Validator::check_method(client_value_type& client_data) {
  std::string method = client_data.request->get_method();

  ws::Location::limit_except_vec_type limit_except_vec = client_data.repository->get_limit_except_vec();
  for (ws::Location::limit_except_vec_type::iterator it = limit_except_vec.begin(); it != limit_except_vec.end(); ++it) {
    if (*it == method)
      return;
  }
  client_data.status = 403;
  //?? 405 error : 메소드는 허용되었지만 실패
}

void ws::Validator::check_uri(client_value_type& client_data) {
  if (client_data.request->get_uri() != client_data.repository->get_dir())
    client_data.status = 404;
  //url & autoindex off -> 403
  return;
}

void ws::Validator::check_version(client_value_type& client_data) {
  if (client_data.request->get_version() != "HTTP/1.1")
    client_data.status = 505;
  return;
}

void check_content_length(ws::Validator::client_value_type& client_data) {
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
    client_data.status = 400;

  if (client_data.request->get_content_length() != client_data.request->get_request_body().length())
    client_data.status = 400;

  return;
}

void check_connection(ws::Validator::client_value_type& client_data) {
  const std::string connection = client_data.request->get_connection();

  if (!(connection == "close" || connection == "keep-alive"))
    client_data.status = 400;
}

void check_transfer_encoding(ws::Validator::client_value_type& client_data) {
  (void) client_data; // todo
}

void check_host(ws::Validator::client_value_type& client_data) {
  std::string host = client_data.request->get_server_name();
  if (host == "")
    client_data.status = 400;
}
