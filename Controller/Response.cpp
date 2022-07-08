#include "Response.hpp"

#include <unistd.h>

#include "Define.hpp"

ws::HeaderGenerator ws::Response::_header_generator;

ws::Response::Response() {}

ws::Response::~Response() {}

void ws::Response::set_data(ws::Socket* socket, client_value_type& client_data, uintptr_t client_fd) {
  _socket = socket;
  _repo = *(client_data.repository);
  _client_fd = client_fd;
}

void ws::Response::set_kernel(Kernel *kernel) {
  _kernel = kernel;
}

// todo: can remove client_data arg
void ws::Response::process(ws::Socket* socket, client_value_type& client_data, uintptr_t client_fd) {
  set_data(socket, client_data, client_fd);

  _kernel->kevent_ctl(
    client_fd,
    EVFILT_USER,
    EV_ADD | EV_ONESHOT,
    NOTE_TRIGGER,
    0,
    reinterpret_cast<void*>(ws::Socket::write_data)
  );

//  if (!_repo.get_autoindex().empty()) {
//    client_data.response.first = this->generate_directory_list();
//    _kernel->kevent_ctl(client_fd, EVFILT_WRITE, EV_ADD, 0, 0, reinterpret_cast<void*>(&ws::Socket::send_response));
//  }
}

void ws::Response::generate(ws::Socket *socket, ws::Response::client_value_type &client_data, uintptr_t client_fd) {
  set_data(socket, client_data, client_fd);
  std::string& response_data = client_data.response.first;

  response_data = _header_generator.generate(CREATED, response_data.length()) + "\r\n" + response_data;
}

std::string ws::Response::generate_directory_list() const {
  std::string body = this->generate_directory_list_body();
  return _header_generator.generate(301, body.length()) + "\r\n" + body; // todo status
}

std::string ws::Response::generate_directory_list_body() const {
  return "https://www.naver.com"; // todo html directory list page
}
