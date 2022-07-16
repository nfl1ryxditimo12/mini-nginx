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
  set_data(client_data, client_fd);

  ws::Repository::redirect_type redirect = client_data.repository.get_redirect();

  if (client_data.status >= BAD_REQUEST)
    _kernel->add_read_event(client_data.repository.get_fd(), reinterpret_cast<void*>(ws::Socket::read_data));
  else if (redirect.first > 0) {
    /*
      redirect.second 값을 어디에 세팅하나
      300 이상: header 설정 후 send
      300 미만: body 설정 후 send
    */
    if (redirect.first < 300)
      client_data.response = redirect.second;

    _kernel->process_event(client_fd, reinterpret_cast<void*>(&Socket::generate_response));
  }
  else {
    if (client_data.repository.get_method() == "DELETE") {
      remove(client_data.repository.get_file_path().c_str());
      _kernel->process_event(client_fd, reinterpret_cast<void*>(&Socket::generate_response));
    }
    else {
      if (client_data.repository.get_cgi().first != "") {
        (void)redirect;
        /* 알아서 cgi 처리해야함 */
      } else if (!client_data.repository.get_autoindex().empty()) {
        ws::Repository::autoindex_type autoindex = client_data.repository.get_autoindex();

        client_data.response += "<html>\n<head>\n</head>\n<body>\n<ul>\n";
        for (ws::Repository::autoindex_type::iterator it = autoindex.begin(); it != autoindex.end(); ++it) {
          client_data.response += ("<li><a href=\"" + *it + "\">" + *it + "</a></li>\n");
        }
        client_data.response += "</ul>\n</body>\n</html>";
        _kernel->process_event(client_fd, reinterpret_cast<void*>(&Socket::generate_response));
      } else if (client_data.repository.get_method() == "HEAD") {
        _kernel->process_event(client_fd, reinterpret_cast<void*>(&Socket::generate_response));
      } else if (client_data.repository.get_method() == "GET") {
        _kernel->add_read_event(client_data.repository.get_fd(), reinterpret_cast<void *>(&Socket::read_data));
      } else {
        _kernel->add_write_event(client_data.repository.get_fd(), reinterpret_cast<void*>(&Socket::write_data));
      }
    }
  }
}

void ws::Response::generate(ws::Response::client_value_type &client_data, uintptr_t client_fd) {
  set_data(client_data, client_fd);

  std::string& response_data = client_data.response;
  std::string response_header = ws::HeaderGenerator::generate(client_data, response_data.length());


  if (client_data.status < 400 && client_data.status >= 300 && client_data.repository.get_redirect().first > 0) // todo: move to generate
    response_header += ("Location: " + client_data.repository.get_redirect().second + "\r\n");



  response_data = response_header + "\r\n" + response_data;

  // std::cout << "test\n" << response_data << "\ntest\n"; //@
}
