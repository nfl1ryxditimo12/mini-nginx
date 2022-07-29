#include "Socket.hpp"

#include <sys/wait.h>

#include "Response.hpp"
#include "Validator.hpp"

ws::Configure ws::Socket::_conf;
ws::Kernel ws::Socket::_kernel;
ws::Socket::server_map_type ws::Socket::_server;
ws::Socket::client_map_type ws::Socket::_client;
ws::Socket::session_map_type ws::Socket::_session;
ws::Validator ws::Socket::_validator;
ws::Response ws::Socket::_response;
sig_atomic_t ws::Socket::_signal = 0;
unsigned int ws::Socket::_session_index = 0;
unsigned int ws::Socket::_accept_index = 0;

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
      exit_socket();
    }
    if (listen(socket_fd, 2048) == -1)
      exit_socket();
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    _server.insert(server_map_type::value_type(socket_fd, host[i]));
    _kernel.add_read_event(socket_fd, reinterpret_cast<void*>(&Socket::connect_client));
  }
}

ws::Socket::~Socket() {}

bool ws::Socket::check_socket_func(kevent_func f) {
  return (
    f == &Socket::recv_request ||
    f == &Socket::process_request ||
    f == &Socket::send_response ||
    f == &Socket::process_session
  );
}

void ws::Socket::run_server() {
  int event_size;

  _kernel.add_signal_event(SIGINT, reinterpret_cast<void*>(&Socket::accecpt_signal));

  signal(SIGINT, SIG_IGN);

  while (true) {

    event_size = _kernel.kevent_ctl(_client.size() + _server.size());

    for (int i = 0; i < event_size; i++) {
      const struct kevent& curr_event = _kernel.get_event_list()[i];
      kevent_func func = reinterpret_cast<kevent_func>(curr_event.udata);

      if (check_socket_func(func) && (curr_event.flags & EV_EOF || curr_event.flags & EV_ERROR))
        disconnect_client(curr_event.ident, curr_event.flags);
      else
        (*func)(curr_event);
    }
  }
}

/* Private function */

void ws::Socket::init_client(unsigned int fd, listen_type listen, const struct sockaddr_in& sock) {
  _client.insert(client_map_type::value_type(fd, client_value_type(listen, sock)));
}

void ws::Socket::disconnect_client(int fd, uint16_t cause) {
  client_map_type::iterator client_iter = _client.find(fd);

  if (client_iter == _client.end())
    return;

  _client.erase(client_iter);

  close(fd);

  std::string err = cause & EV_EOF ? "EOF" : cause & EV_ERROR ? "ERROR" : "";
  ws::Util::print_disconnect_client(fd, err);
}

void ws::Socket::exit_socket() {
  exit(1);
}

void ws::Socket::accecpt_signal(struct kevent event) {
  (void)event;
  exit(0);
}

void ws::Socket::connect_client(struct kevent event) {
  if (_signal == SIGINT)
    return;

  listen_type& listen = _server.find(event.ident)->second;
  int client_socket_fd;
  struct sockaddr_in client_info;
  int struct_size = sizeof(client_info);

  for (int i = 0; i < event.data; ++i) {
    client_socket_fd = accept(event.ident, (struct sockaddr*)&client_info, (socklen_t *)&struct_size);

    if (client_socket_fd != -1) {
      fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
      init_client(client_socket_fd, listen, client_info);

      ++_accept_index;

      _kernel.add_read_event(client_socket_fd, reinterpret_cast<void *>(&Socket::recv_request));
    }
  }
}

void ws::Socket::recv_request(struct kevent event) {
  client_value_type& client_data = _client.find(event.ident)->second;

  client_data.buffer.init_buf();
  ssize_t read_size;
  read_size = client_data.buffer.read_file(event.ident);

  if (read_size == -1 || client_data.request.eof()) {
    _kernel.delete_read_event(event.ident);
    disconnect_client(event.ident, EV_ERROR);
    return;
  }

  if (read_size > 0)
    client_data.status = client_data.request.parse_request_message(_conf, &client_data.buffer, client_data.repository);

  if (read_size == 0) {
    _kernel.delete_read_event(event.ident);
    disconnect_client(event.ident, EV_ERROR);
    return;
  }

  if (client_data.request.eof() || client_data.status || !read_size) {
    ws::Util::print_accept_client((client_data.request.get_method() + " " + client_data.request.get_uri()) , _accept_index, event.ident);
    _kernel.delete_read_event(event.ident);
    _kernel.add_user_event(event.ident, reinterpret_cast<void *>(&Socket::process_request), EV_ONESHOT);
    client_data.buffer.clear();
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

  if (client_data.fatal) {
    close(client_data.repository.get_fd());
    disconnect_client(event.ident, EV_ERROR);
    return;
  }

  _response.process(client_data, event.ident);
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

  if (it != _session.end())
    client_data.request.set_session_id(it->first);

  if (method == "GET") {
    ++it->second.hit_count;
    client_data.response_body += "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<title>session</title>\n</head>\n<body>\n<h4>hit point: ";
    client_data.response_body += Util::ultos(it->second.hit_count);
    client_data.response_body += "</h4>\n<h4>name: ";
    client_data.response_body += it->second.name;
    client_data.response_body += "</h4>\n<h4>id: ";
    client_data.response_body += Util::ultos(it->first);
    client_data.response_body +="</h4>\n</body>\n</html>\n";
  }
  else if (method == "POST") {
    if (_session.size() > 300)
      _session.clear();
    ++_session_index;
    _session.insert(session_map_type::value_type(_session_index, session_value_type(client_data.request.get_name())));
    client_data.request.set_session_id(_session_index);
  }
  else if (method == "DELETE")
    _session.erase(it);

  ws::Socket::generate_response(event.ident, client_data);
}

void ws::Socket::read_data(struct kevent event) {
  const client_map_type::iterator& client = find_client_by_file(event.ident);

  char buffer[kBUFFER_SIZE + 1];
  ssize_t read_size = 0;

  read_size = read(client->second.repository.get_fd(), buffer, kBUFFER_SIZE);

  if (read_size <= 0) {
    _kernel.delete_read_event(event.ident);
    close(event.ident);
    disconnect_client(event.ident, EV_ERROR);
    return;
  }

  client->second.response_body.insert(client->second.response_body.length(), buffer, read_size);

  if (Util::is_eof(event.ident)) {
    _kernel.delete_read_event(event.ident);
    close(event.ident);
    ws::Socket::generate_response(client->first, client->second);
  }
}

void ws::Socket::write_data(struct kevent event) {
  const client_map_type::iterator& client = find_client_by_file(event.ident);
  const std::string& request_body = client->second.request.get_request_body();
  std::size_t& offset = client->second.write_offset;

  ssize_t write_size = write(client->second.repository.get_fd(), request_body.c_str() + offset, request_body.length() - offset);

  if (write_size == -1) {
    _kernel.delete_write_event(event.ident);
    close(event.ident);
    disconnect_client(event.ident, EV_ERROR);
    return;
  }

  offset += write_size;

  if (offset == request_body.length()) {
    offset = 0;
    _kernel.delete_write_event(event.ident);
    close(event.ident);
    ws::Socket::generate_response(client->first, client->second);
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
  ssize_t read_size = client->second.buffer.read_file(event.ident);

  if (read_size < 0) {
    _kernel.delete_read_event(event.ident);
    disconnect_client(client->first, EV_ERROR);
    return;
  }
  if (read_size == 0) {
    _kernel.delete_read_event(event.ident);
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

void ws::Socket::write_pipe(struct kevent event) {
  const client_map_type::iterator& client = find_client_by_fpipe(event.ident);
  const std::string& request_body = client->second.request.get_request_body();
  std::string::size_type& offset = client->second.pipe_offset;
  ssize_t write_size = write(event.ident, request_body.c_str() + offset, request_body.length() - offset);

  if (write_size <= 0) {
    _kernel.delete_write_event(event.ident);
    disconnect_client(client->first, EV_ERROR);
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
}

void ws::Socket::send_response(struct kevent event) {
  client_value_type& client_data = _client.find(event.ident)->second;
  const std::string& response_data = client_data.response_total;
  std::string::size_type& offset = client_data.write_offset;

  if (!client_data.cgi_handler.get_eof())
    return;

  ssize_t n = write(event.ident, response_data.c_str() + offset, response_data.length() - offset);
  if (n <= 0) {
    _kernel.delete_write_event(event.ident);
    disconnect_client(event.ident, EV_ERROR);
    return;
  }

  offset += n;

  if (offset == response_data.length()) {
    _kernel.delete_write_event(event.ident);
    ws::Util::print_response_client(client_data.status, event.ident);
    disconnect_client(event.ident);
  }
}

ws::Socket::session_map_type& ws::Socket::get_session() throw() {
  return _session;
}
