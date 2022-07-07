#include "Response.hpp"

ws::HeaderGenerator ws::Response::_header_generator;

ws::Response::Response() {}

ws::Response::~Response() {}

void ws::Response::init_response(ws::Socket* socket, client_value_type& client_data, uintptr_t client_fd) {
  _socket = socket;
  _repo = *(client_data.repository);
  _client_fd = client_fd;
}

void ws::Response::set_kernel(Kernel *kernel) {
  _kernel = kernel;
}

std::string ws::Response::generate_directory_list() const throw() {
  return _header_generator.generate_data + "temp"; // todo make string return
}

void ws::Response::generate_response(ws::Socket* socket, client_value_type& client_data, uintptr_t client_fd) {
  init_response(socket, client_data, client_fd);

  if (!_repo.get_autoindex().empty()) {
    client_data.response = this->generate_directory_list();
    _kernel->kevent_ctl(client_fd, EVFILT_WRITE, EV_ADD, 0, 0, reinterpret_cast<void*>(&ws::Socket::send_response));
  }
}
