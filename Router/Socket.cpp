#include "Socket.hpp"

#include <sys/wait.h>

#include "Response.hpp"
#include "Validator.hpp"

/* console test code */ // todo: remove
#include <iostream>
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"
/* console test code */

ws::Configure ws::Socket::_conf;
ws::Kernel ws::Socket::_kernel;
ws::Socket::server_map_type ws::Socket::_server;
ws::Socket::client_map_type ws::Socket::_client;
ws::Socket::session_map_type ws::Socket::_session;
ws::Validator ws::Socket::_validator;
ws::Response ws::Socket::_response;
sig_atomic_t ws::Socket::_signal = 0;
unsigned int ws::Socket::_session_index = 0;

const std::size_t ws::Socket::kBUFFER_SIZE = 1024 * 1024;

void ws::Socket::init_server(const ws::Configure& conf) {
  _conf = conf;
  _response.set_kernel(&_kernel);

  ws::Configure::listen_vec_type host = conf.get_host_list();

  for (size_t i = 0; i < host.size(); i++) {
    int                 socket_fd;
    struct sockaddr_in  addr_info;

    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
      exit_socket();

    int k = true;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &k, sizeof(k));

    memset(&addr_info, 0, sizeof(addr_info));
    addr_info.sin_family = AF_INET;
    addr_info.sin_addr.s_addr = host[i].first;
    addr_info.sin_port = host[i].second;

    if (bind(socket_fd, (struct sockaddr*)&addr_info, sizeof(addr_info)) == -1) {
      std::cout << strerror(errno) << std::endl;
        exit_socket();
    }
    if (listen(socket_fd, 2000) == -1)
      exit_socket();
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    _server.insert(server_map_type::value_type(socket_fd, host[i]));
    _kernel.add_read_event(socket_fd, reinterpret_cast<void*>(&Socket::connect_client));
  }
}

/* 시그널 처리 알아보고 적용해야함 */
// sig_atomic_t received_sig;

// void handler(int sig) {
//   received_sig = sig;
// }

// int main() {
//   while (true) {
//     process_event();
//     if (received_sig) {
//       return;
//     }
//   }
// }

ws::Socket::~Socket() {}

void ws::Socket::run_server() {
  size_t event_size = _server.size() * 8;
  struct kevent event_list[event_size];
  int new_event;

  _kernel.add_signal_event(SIGINT, reinterpret_cast<void*>(&Socket::accecpt_signal));

  signal(SIGINT, SIG_IGN);

  while (true) {

    if (_signal == SIGINT && _client.empty())
      exit(0);

    new_event = _kernel.kevent_wait(event_list, event_size);

    for (int i = 0; i < new_event; i++) {
      // todo EOF 처리 깔끔하게
//      if (event_list[i].flags & EV_EOF) {
//        std::cout << "EOF" << std::endl; // todo: test print
//        disconnect_client(event_list[i].ident);
//      } else {
        kevent_func func = reinterpret_cast<kevent_func>(event_list[i].udata);
        (*func)(event_list[i]);
//      }
    }
  }
}

/* Private function */

void ws::Socket::init_client(unsigned int fd, listen_type listen) {
  _client.insert(client_map_type::value_type(fd, client_value_type(listen)));
}

void ws::Socket::disconnect_client(int fd) {
  client_map_type::iterator client_iter = _client.find(fd);

  if (client_iter == _client.end())
    return;

  _client.erase(client_iter);

  close(fd);
}

void ws::Socket::exit_socket() {
  exit(1);
}

void ws::Socket::accecpt_signal(struct kevent event) {
  if (event.ident == SIGINT)
    _signal = SIGINT;
}

void ws::Socket::connect_client(struct kevent event) {
  if (_signal == SIGINT)
    return;
  listen_type& listen = _server.find(event.ident)->second;
  int client_socket_fd;

  if ((client_socket_fd = accept(event.ident, NULL, NULL)) == -1)
    return;

  fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
  init_client(client_socket_fd, listen);
  _kernel.add_read_event(client_socket_fd, reinterpret_cast<void*>(&Socket::recv_request));
}

void ws::Socket::recv_request(struct kevent event) {
  client_value_type& client_data = _client.find(event.ident)->second;

  client_data.buffer.init_buf();
  ssize_t read_size;
  read_size = client_data.buffer.read_file(event.ident);

  if (read_size == -1) {
    _kernel.delete_read_event(event.ident);
    disconnect_client(event.ident);
    return;
  }

  if (client_data.request.eof() && read_size > 0) // todo session
    client_data.request.clear();

  if (read_size > 0)
    client_data.status = client_data.request.parse_request_message(_conf, &client_data.buffer, client_data.repository);

//  todo
  if (read_size == 0) {
    _kernel.delete_read_event(event.ident);
    disconnect_client(event.ident);
//     operation timeout
    return;
  }

  if (client_data.request.eof() || client_data.status || !read_size) {
//    client_data.request.test(); // todo: test print
//    std::cout << YLW << "\n=================================================\n" << NC << std::endl;
    _kernel.add_user_event(event.ident, reinterpret_cast<void *>(&Socket::process_request), EV_ONESHOT);
    _kernel.delete_read_event(event.ident);
    client_data.buffer.clear();
    ws::Util::print_running_time("recv_request()", client_data.start_time);
  }
}

void ws::Socket::process_request(struct kevent event) {
  client_value_type& client_data = _client.find(event.ident)->second;

  if (!client_data.status)
    _validator(_session, client_data);

  std::string::size_type extension_dot = client_data.request.get_uri().find_last_of('.');
  const std::string& extension = client_data.request.get_uri().substr(extension_dot + 1);
  const ws::Location::cgi_map_type& cgi_map = client_data.repository.get_location()->get_cgi_map();
  const ws::Location::cgi_map_type::const_iterator& name = cgi_map.find(extension);

  if (client_data.request.get_method() == "POST" && extension_dot != std::string::npos && name != cgi_map.end()) {
    close(client_data.repository.get_fd());

    ws::Request::header_type::const_iterator special = client_data.request.get_request_header().find("X-Secret-Header-For-Test");
    if (special != client_data.request.get_request_header().end() && setenv(("HTTP_" + special->first).c_str(), special->second.c_str(), 1)) {
      client_data.status = INTERNAL_SERVER_ERROR;
    } else {
      pid_t pid = client_data.cgi_handler.run_cgi(
        client_data.repository.get_method().c_str(),
        name->second.c_str(),
        name->second.c_str(),
        _kernel
      );

      if (pid != -1) {
        client_data.cgi_pid = pid;
        _kernel.add_process_event(pid, reinterpret_cast<void*>(ws::Socket::wait_child), 0, NOTE_EXIT | NOTE_SIGNAL);
        return;
      }

      client_data.status = INTERNAL_SERVER_ERROR;
    }
  }

  client_data.buffer.delete_buf();

  client_data.repository.set_repository(client_data.status);
  client_data.status = client_data.repository.get_status();
  client_data.fatal = client_data.repository.is_fatal();

  std::cout << "request body len: " << client_data.request.get_request_body().length() << std::endl; // todo: test print

  if (client_data.fatal) {
    close(client_data.repository.get_fd());
    disconnect_client(event.ident);
    return;
  }

  // todo
//  if (client_data.repository.get_location()->get_block_name() == "/directory" && client_data.repository.get_method() == "POST")
//    for (size_t i = 0; i < client_data.request.get_request_body().length(); ++i)
//      client_data.response.push_back(static_cast<int>(std::toupper(client_data.request.get_request_body()[i])));

  _response.process(client_data, event.ident);
  ws::Util::print_running_time("process_request()", client_data.start_time);
}

ws::Socket::client_map_type::iterator ws::Socket::find_client_by_file(int file) throw() {
  client_map_type::iterator curr = _client.begin();
  client_map_type::iterator end = _client.end();

  while (curr != end && curr->second.repository.get_fd() != file)
    ++curr;

  return curr;
}

void ws::Socket::process_session(struct kevent event) {
  client_value_type& client_data = _client.find(event.ident)->second;
  const std::string& method = client_data.repository.get_method();
  session_map_type::iterator it = _session.find(client_data.request.get_session_id());

  /* todo:
   * - GET일때 세션아이디 검색해서 존재하면 html에 추가해서 띄워주기
   * - POST일때 세션아이디 ++해서 insert 해주기
   * - DELETE일때 세션아이디 검색해서 지우기
   */
  if (it != _session.end())
    client_data.request.set_session_id(it->first);

  if (method == "GET") {
    ++it->second.hit_count;
    // todo: response
    client_data.response_body += "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<title>session</title>\n</head>\n<body>\n<h4>hit point: ";
    client_data.response_body += Util::ultos(it->second.hit_count);
    client_data.response_body += "</h4>\n<h4>name: ";
    client_data.response_body += it->second.name;
    client_data.response_body += "</h4>\n<h4>id: ";
    client_data.response_body += Util::ultos(it->first);
    client_data.response_body +="</h4>\n</body>\n</html>";
  }
  else if (method == "POST") {
    // todo 이 조건은 사라져도 될 듯?
    if (_session.size() > 300)
      _session.clear();
    ++_session_index;
    _session.insert(session_map_type::value_type(_session_index, session_value_type(client_data.request.get_name())));
    client_data.request.set_session_id(_session_index);
  }
  else if (method == "DELETE")
    _session.erase(it);

  ws::Socket::generate_response(event.ident, client_data);
  ws::Util::print_running_time("process_session()", client_data.start_time);
}

void ws::Socket::read_data(struct kevent event) {
  const client_map_type::iterator& client = find_client_by_file(event.ident);

  char buffer[kBUFFER_SIZE + 1];
  ssize_t read_size = 0;

  read_size = read(client->second.repository.get_fd(), buffer, kBUFFER_SIZE);

  if (read_size <= 0) { // todo: read 0 is an error?
    std::cerr << "Socket: read error occurred" << std::endl;
    _kernel.delete_read_event(event.ident);
    close(event.ident);
    disconnect_client(event.ident);
    return;
  }

  client->second.response_body.insert(client->second.response_body.length(), buffer, read_size);

  if (Util::is_eof(event.ident)) {
    _kernel.delete_read_event(event.ident);
    close(event.ident);
    ws::Socket::generate_response(client->first, client->second);
    ws::Util::print_running_time("read_data()", client->second.start_time);
  }
}

void ws::Socket::write_data(struct kevent event) {
  const client_map_type::iterator& client = find_client_by_file(event.ident);
  const std::string& request_body = client->second.request.get_request_body();
  std::size_t& offset = client->second.write_offset;

  ssize_t write_size = write(client->second.repository.get_fd(), request_body.c_str() + offset, request_body.length() - offset); // todo event.data

  if (write_size == -1) {
    std::cerr << "Socket: write error occurred" << std::endl;
    _kernel.delete_write_event(event.ident);
    close(event.ident);
    disconnect_client(event.ident);
    return;
  }

  offset += write_size;

  if (offset == request_body.length()) {
    offset = 0;
    _kernel.delete_write_event(event.ident); // todo?
    close(event.ident);
    ws::Socket::generate_response(client->first, client->second);
    ws::Util::print_running_time("write_data()", client->second.start_time);
  }
}

ws::Socket::client_map_type::iterator ws::Socket::find_client_by_fpipe(int fpipe) throw() {
  client_map_type::iterator curr = _client.begin();
  client_map_type::iterator end = _client.end();

  while (curr != end && curr->second.cgi_handler.get_fpipe()[1] != fpipe)
    ++curr;

  return curr;
}

ws::Socket::client_map_type::iterator ws::Socket::find_client_by_bpipe(int bpipe) throw() {
  client_map_type::iterator curr = _client.begin();
  client_map_type::iterator end = _client.end();

  while (curr != end && curr->second.cgi_handler.get_bpipe()[0] != bpipe)
    ++curr;

  return curr;
}

void ws::Socket::parse_cgi_return(client_value_type &client) {
  while (client.is_cgi_header) {
    ws::Token token;
    token.rd_http_line(client.buffer);
    ws::Token::size_type pos = token.find(':');

    if (token == "\r\n") {
      client.is_cgi_header = false;
      break;
    }

    if (pos == ws::Token::npos) {
      client.buffer << token;
      return;
    }

    if (!token.compare(0, pos, "Status")) {
      client.response_header =
        "HTTP/1.1 "
        + token.substr(pos + 2, token.find(' ', pos + 2) - (pos + 2))
        + "\r\n"
      ;
    } else {
      client.response_header += token;
    }

    if (client.buffer.eof())
      return;
  }

  client.response_body << client.buffer;
}

void ws::Socket::read_pipe(struct kevent event) {
  const client_map_type::iterator& client = find_client_by_bpipe(event.ident);
  ssize_t read_size = client->second.buffer.read_file(event.ident, event.data);

  if (read_size < 0) {
    std::cerr << event.data << std::endl; // test print
    std::cerr << read_size << std::endl; // test print
    std::cerr << client->second.response_body.length() << std::endl; // test print

    std::cerr << (event.flags & EV_EOF ? "EOF" : "NO") << std::endl;

    std::cerr << "Socket: pipe read error occurred" << std::endl;
    _kernel.delete_read_event(event.ident);
    disconnect_client(client->first);
    return;
  }
  if (read_size == 0) {
    std::cerr << event.data << std::endl; // test print
    std::cerr << read_size << std::endl; // test print
    std::cerr << client->second.response_body.length() << std::endl; // test print
    return;
  }
  parse_cgi_return(client->second);

  if (client->second.response_body.length() == client->second.request.get_request_body().length()) {
    client->second.buffer.delete_buf();
    _kernel.delete_read_event(event.ident);
    close(event.ident);
    client->second.response_total = client->second.response_header + "\r\n" + client->second.response_body;
    _kernel.add_write_event(client->first, reinterpret_cast<void*>(ws::Socket::send_response));
  }
}

void ws::Socket::write_pipe(struct kevent event) { // todo: when refactoring is done, call read and write both
  const client_map_type::iterator& client = find_client_by_fpipe(event.ident);
  const std::string& request_body = client->second.request.get_request_body();
  std::string::size_type& offset = client->second.pipe_offset;
  ssize_t write_size = write(event.ident, request_body.c_str() + offset, std::min(static_cast<std::size_t>(event.data), request_body.length() - offset));

  if (write_size <= 0) {
    std::cerr << "Socket: pipe write error occurred" << std::endl;
    _kernel.delete_write_event(event.ident);
    disconnect_client(client->first);
    return;
  }

  offset += write_size;

  if (offset == request_body.length()) {
    offset = 0;
    _kernel.delete_write_event(event.ident);
    close(event.ident);
  }
}

ws::Socket::client_map_type::iterator ws::Socket::find_client_by_pid(pid_t pid) throw() {
  client_map_type::iterator ret = _client.begin();

  while (ret != _client.end() && ret->second.cgi_pid != pid)
    ++ret;

  return ret;
}

void ws::Socket::wait_child(struct kevent event) {
  client_value_type& client = find_client_by_pid(event.ident)->second;

  if (waitpid(event.ident, NULL, 0))
    client.cgi_handler.set_eof(true);
}

void ws::Socket::generate_response(int client_fd, client_value_type& client_data) {
  _response.generate(client_data);
  _kernel.add_write_event(client_fd, reinterpret_cast<void*>(ws::Socket::send_response));
  ws::Util::print_running_time("generate_response()", client_data.start_time);
}

void ws::Socket::send_response(struct kevent event) {
  client_value_type& client_data = _client.find(event.ident)->second;
  const std::string& response_data = client_data.response_total;
  std::string::size_type& offset = client_data.write_offset;
  if (!client_data.cgi_handler.get_eof())
    return;

  ssize_t n;
  if ((n = write(event.ident, response_data.c_str() + offset, response_data.length() - offset)) == -1) { // todo: event.data
    _kernel.delete_write_event(event.ident);
    disconnect_client(event.ident); // todo: close
    return;
  }

  offset += n;

  if (offset == response_data.length()) {
    ws::Util::print_running_time("send_response()", client_data.start_time);
    std::cout << YLW << "\n=========================================================\n" << NC << std::endl;
    _kernel.delete_write_event(event.ident);
    disconnect_client(event.ident);
  }
}
