#include "Response.hpp"

#include <unistd.h>

#include "Define.hpp"


ws::Response::Response() {}

ws::Response::~Response() {}

void ws::Response::set_data(ws::Socket* socket, client_value_type& client_data, uintptr_t client_fd) {
  _socket = socket;
  _repo = &client_data.repository;
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
    EV_ADD,
    NOTE_TRIGGER,
    0,
    reinterpret_cast<void*>(client_data.request.get_method() == "POST" ? ws::Socket::write_data : ws::Socket::read_data)
  );

//  if (!_repo.get_autoindex().empty()) {
//    client_data.response.first = this->generate_directory_list();
//    _kernel->kevent_ctl(client_fd, EVFILT_WRITE, EV_ADD, 0, 0, reinterpret_cast<void*>(&ws::Socket::send_response));
//  }
}

void ws::Response::generate(ws::Socket *socket, ws::Response::client_value_type &client_data, uintptr_t client_fd) {
  set_data(socket, client_data, client_fd);

  std::string& response_data = client_data.response;
  std::string response_header = ws::HeaderGenerator::generate(OK, response_data.length());
  response_data = response_header + "\r\n" + response_data;
}

std::string ws::Response::generate_directory_list() const {
  std::string body = this->generate_directory_list_body();
  return ws::HeaderGenerator::generate(301, body.length()) + "\r\n" + body; // todo status
}

std::string ws::Response::generate_directory_list_body() const {
  return "https://www.naver.com"; // todo html directory list page
}
