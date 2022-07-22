#include "Response.hpp"

#include <unistd.h>

#include "Define.hpp"

ws::Response::Response() throw() {}

ws::Response::~Response() {}

//void ws::Response::set_data(client_value_type& client_data, uintptr_t client_fd) {
//  _repo = &client_data._repository;
//  _client_fd = client_fd;
//}

void ws::Response::set_kernel(Kernel *kernel) {
  _kernel = kernel;
}

// todo: can remove client_data arg
// todo: need to add HEAD method
void ws::Response::process(client_value_type& client_data, uintptr_t client_fd) {
//0. set_data
//  set_data(client_data, client_fd);
  _repo = &client_data._repository;
  ws::Repository::redirect_type redirect = client_data._repository.get_redirect();

//1. 400error
  if (client_data._status >= BAD_REQUEST) {
    _kernel->add_read_event(client_data._repository.get_fd(), reinterpret_cast<void*>(ws::Socket::read_data));
    return;
  }

//2. session
  if (_repo->is_session()) {
    _kernel->add_user_event(client_fd, reinterpret_cast<void *>(&Socket::process_session), EV_ONESHOT);
    return;
  }

//3. redirect
  if (redirect.first > 0) {
    if (redirect.first < 300 && client_data._repository.get_method() != "HEAD")
      client_data._response = redirect.second;

    _kernel->add_user_event(client_fd, reinterpret_cast<void *>(&Socket::generate_response), EV_ONESHOT);
    return;
  }

//4. autoindex
  if (!client_data._repository.get_autoindex().empty()) {
    ws::Repository::autoindex_type autoindex = client_data._repository.get_autoindex();

    client_data._response += "<html>\n<head>\n</head>\n<body>\n<ul>\n";
    for (ws::Repository::autoindex_type::iterator it = autoindex.begin(); it != autoindex.end(); ++it) {
      client_data._response += ("<li><a href=\"" + *it + "\">" + *it + "</a></li>\n");
    }
    client_data._response += "</ul>\n</body>\n</html>";
    _kernel->add_user_event(client_fd, reinterpret_cast<void *>(&Socket::generate_response), EV_ONESHOT);
    return;
  }
  
//5. delete method
  if (client_data._repository.get_method() == "DELETE") {
    remove(client_data._repository.get_file_path().c_str());
    _kernel->add_user_event(client_fd, reinterpret_cast<void *>(&Socket::generate_response), EV_ONESHOT);
    return;
  }

//6. head method
  if (client_data._repository.get_method() == "HEAD") {
    _kernel->add_user_event(client_fd, reinterpret_cast<void *>(&Socket::generate_response), EV_ONESHOT);
  } else if (client_data._repository.get_method() == "GET") {
    if (Util::is_eof(client_data._repository.get_fd())) {
      close(client_data._repository.get_fd());
      _kernel->add_user_event(client_fd, reinterpret_cast<void *>(Socket::generate_response), EV_ONESHOT);
    } else
      _kernel->add_read_event(client_data._repository.get_fd(), reinterpret_cast<void*>(&Socket::read_data));
  } else {
    _kernel->add_write_event(client_data._repository.get_fd(), reinterpret_cast<void*>(&Socket::write_data));
  }
}

void ws::Response::generate(ws::Response::client_value_type &client_data, uintptr_t client_fd) {
//  set_data(client_data, client_fd);
  (void) client_fd;
  _repo = &client_data._repository;

  std::string& response_data = client_data._response;
  std::string response_header = ws::HeaderGenerator::generate(client_data, response_data.length());

  response_data = response_header + "\r\n" + response_data;

  // std::cout << "test\n" << response_data << "\ntest\n"; // todo: test print
}
