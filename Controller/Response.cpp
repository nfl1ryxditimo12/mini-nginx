#include "Response.hpp"

#include <unistd.h>

#include "Define.hpp"

ws::Response::Response() throw() {}

ws::Response::~Response() {}

void ws::Response::set_data(client_value_type& client_data, uintptr_t client_fd) {
  _repo = &client_data.repository;
  _client_fd = client_fd;
}

void ws::Response::set_kernel(Kernel *kernel) {
  _kernel = kernel;
}

// todo: can remove client_data arg
// todo: need to add HEAD method
void ws::Response::process(client_value_type& client_data, uintptr_t client_fd) {
  //4-2. cgi
  if (client_data.repository.get_cgi().first != "") {
    close(client_data.repository.get_fd());
    std::string temp("/goinfre/jaham/webserv");
    if (!client_data.cgi_handler.run_cgi(
      client_data.repository.get_method().c_str(),
      temp.c_str(),
      (temp + client_data.repository.get_location()->get_cgi()).c_str(),
      _kernel
    )) {
      client_data.status = INTERNAL_SERVER_ERROR;
      close(client_data.repository.get_fd());
      client_data.repository.set_fd(open((Util::get_root_dir() + "/www/500.html").c_str(), O_RDONLY));
      _kernel->add_read_event(client_data.repository.get_fd(), reinterpret_cast<void*>(ws::Socket::read_data));
    }
//    if (!client_data.cgi_handler.run_cgi(
//      client_data.repository.get_method().c_str(),
//      client_data.repository.get_cgi_path().c_str(),
//      (client_data.repository.get_cgi_path() + client_data.repository.get_location()->get_cgi()).c_str(),
//      _kernel
//    )) {
//      client_data.status = INTERNAL_SERVER_ERROR;
//      close(client_data.repository.get_fd());
//      client_data.repository.set_fd(open((Util::get_root_dir() + "/www/500.html").c_str(), O_RDONLY));
//      _kernel->add_read_event(client_data.repository.get_fd(), reinterpret_cast<void*>(ws::Socket::read_data));
//    }

    return;
  }

//0. set_data
  set_data(client_data, client_fd);
  ws::Repository::redirect_type redirect = client_data.repository.get_redirect();

//1. 400error
  if (client_data.status >= BAD_REQUEST) {
    _kernel->add_read_event(client_data.repository.get_fd(), reinterpret_cast<void*>(ws::Socket::read_data));
    return;
  }

//2. redirect
  if (redirect.first > 0) {
    if (redirect.first < 300 && client_data.repository.get_method() != "HEAD")
      client_data.response = redirect.second;

    _kernel->add_process_event(client_fd, reinterpret_cast<void *>(&Socket::generate_response), EV_ONESHOT);
    return;
  }

//3. autoindex
  if (!client_data.repository.get_autoindex().empty()) {
    ws::Repository::autoindex_type autoindex = client_data.repository.get_autoindex();

    client_data.response += "<html>\n<head>\n</head>\n<body>\n<ul>\n";
    for (ws::Repository::autoindex_type::iterator it = autoindex.begin(); it != autoindex.end(); ++it) {
      client_data.response += ("<li><a href=\"" + *it + "\">" + *it + "</a></li>\n");
    }
    client_data.response += "</ul>\n</body>\n</html>";
    _kernel->add_process_event(client_fd, reinterpret_cast<void *>(&Socket::generate_response), EV_ONESHOT);
    return;
  }
  
//4. delete method
  if (client_data.repository.get_method() == "DELETE") {
    remove(client_data.repository.get_file_path().c_str());
    _kernel->add_process_event(client_fd, reinterpret_cast<void *>(&Socket::generate_response), EV_ONESHOT);
    return;
  }

//4. head method
  if (client_data.repository.get_method() == "HEAD") {
    _kernel->add_process_event(client_fd, reinterpret_cast<void *>(&Socket::generate_response), EV_ONESHOT);
  } else if (client_data.repository.get_method() == "GET") {
    if (Util::is_eof(client_data.repository.get_fd())) {
      close(client_data.repository.get_fd());
      _kernel->add_process_event(client_fd, reinterpret_cast<void *>(Socket::generate_response), EV_ONESHOT);
    } else
      _kernel->add_read_event(client_data.repository.get_fd(), reinterpret_cast<void*>(&Socket::read_data));
  } else {
    _kernel->add_write_event(client_data.repository.get_fd(), reinterpret_cast<void*>(&Socket::write_data));
  }
}

void ws::Response::generate(ws::Response::client_value_type &client_data, uintptr_t client_fd) {
  set_data(client_data, client_fd);

  std::string& response_data = client_data.response;
  std::string response_header = ws::HeaderGenerator::generate(client_data, response_data.length());

  response_data = response_header + "\r\n" + response_data;

  // std::cout << "test\n" << response_data << "\ntest\n"; //todo: test print
}
