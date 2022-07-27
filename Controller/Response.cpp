#include "Response.hpp"

#include <unistd.h>

#include "Define.hpp"

ws::Response::Response() throw() {}

ws::Response::~Response() {}

void ws::Response::set_kernel(Kernel *kernel) {
  _kernel = kernel;
}

// todo: can remove client_data arg
// todo: need to add HEAD method
void ws::Response::process(client_value_type& client_data, uintptr_t client_fd) {
//0. set_data
  ws::Repository::redirect_type redirect = client_data.repository.get_redirect();

//1. 400error
  if (client_data.status >= BAD_REQUEST) {
    _kernel->add_read_event(client_data.repository.get_fd(), reinterpret_cast<void*>(ws::Socket::read_data));
    return;
  }

//2. session
  if (client_data.repository.is_session()) {
    _kernel->add_user_event(client_fd, reinterpret_cast<void *>(&Socket::process_session), EV_ONESHOT);
    return;
  }

//3. redirect
  if (redirect.first > 0) {
    if (redirect.first < 300 && client_data.repository.get_method() != "HEAD")
      client_data.response_body = redirect.second;

    ws::Socket::generate_response(client_fd, client_data);
    return;
  }

//4. autoindex
  if (!client_data.repository.get_autoindex().empty()) {
    ws::Repository::autoindex_type autoindex = client_data.repository.get_autoindex();

    client_data.response_body += "<html>\n<head>\n</head>\n<body>\n<ul>\n";
    for (ws::Repository::autoindex_type::iterator it = autoindex.begin(); it != autoindex.end(); ++it) {
      client_data.response_body += ("<li><a href=\"" + *it + "\">" + *it + "</a></li>\n");
    }
    client_data.response_body += "</ul>\n</body>\n</html>";
    ws::Socket::generate_response(client_fd, client_data);
    return;
  }
  
//5. delete method
  if (client_data.repository.get_method() == "DELETE") {
    remove(client_data.repository.get_file_path().c_str());
    ws::Socket::generate_response(client_fd, client_data);
    return;
  }

//6. head method
  if (client_data.repository.get_method() == "HEAD") {
    ws::Socket::generate_response(client_fd, client_data);
  } else if (client_data.repository.get_method() == "GET") {
    if (Util::is_eof(client_data.repository.get_fd())) {
      close(client_data.repository.get_fd());
      ws::Socket::generate_response(client_fd, client_data);
    } else
      _kernel->add_read_event(client_data.repository.get_fd(), reinterpret_cast<void*>(&Socket::read_data));
  } else {
    _kernel->add_write_event(client_data.repository.get_fd(), reinterpret_cast<void*>(&Socket::write_data));
  }
}

void ws::Response::generate(ws::Response::client_value_type &client_data) {
  client_data.response_header = ws::HeaderGenerator::generate(client_data);
  client_data.response_total = client_data.response_header + "\r\n" + client_data.response_body;
}
