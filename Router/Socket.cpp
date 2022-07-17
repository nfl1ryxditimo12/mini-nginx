#include "Socket.hpp"

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
ws::Validator ws::Socket::_validator;
ws::Response ws::Socket::_response;

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
    if (listen(socket_fd, 200) == -1)
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

  while (true) {
    new_event = _kernel.kevent_wait(event_list, event_size);

    for (int i = 0; i < new_event; i++) {
      kevent_func func = reinterpret_cast<kevent_func>(event_list[i].udata);
      (*func)(event_list[i]);
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

//  close(client_iter->second.repository.get_fd());// todo why..?
  _client.erase(client_iter);

  close(fd);
}

void ws::Socket::exit_socket() {
  exit(1);
}

void ws::Socket::connect_client(struct kevent event) {
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
  char buffer[kBUFFER_SIZE + 1];

  ssize_t read_size;
  read_size = read(event.ident, buffer, std::min(static_cast<long>(kBUFFER_SIZE), event.data));

  if (read_size == -1) {
    _kernel.delete_read_event(event.ident);
    disconnect_client(event.ident);
    return;
  }

  buffer[read_size] = 0;
  // std::cout << "input" << std::endl;
  // std::cout << buffer << std::endl; // todo;
  // std::cout << "input end" << std::endl;

  if (client_data.request.eof() && read_size > 0) // todo session
    client_data.request.clear();

  if (read_size > 0) {
//    std::cout << YLW << "\n== Request ======================================\n" << NC << std::endl;  // todo: test print
//    std::cout << buffer << std::endl;
//    std::cout << RED << "\n== Parsing ======================================\n" << NC << std::endl;
    client_data.status = client_data.request.parse_request_message(_conf, buffer, client_data.repository);
  }

//  todo
  if (read_size == 0) {
    _kernel.delete_read_event(event.ident);
    disconnect_client(event.ident);
//     operation timeout
  }

  if (client_data.request.eof() || client_data.status || !read_size) {
//    client_data.request.test(); // todo: test print
//    std::cout << YLW << "\n=================================================\n" << NC << std::endl;
    _kernel.process_event(event.ident, reinterpret_cast<void*>(&Socket::process_request));
    _kernel.delete_read_event(event.ident);
  }
}

void ws::Socket::process_request(struct kevent event) {
  client_value_type& client_data = _client.find(event.ident)->second;

  if (!client_data.status)
    _validator(client_data);

  client_data.repository.set_repository(client_data.status);
  client_data.status = client_data.repository.get_status();
  client_data.fatal = client_data.repository.is_fatal();

  if (client_data.fatal) {
    disconnect_client(event.ident);
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

void ws::Socket::read_data(struct kevent event) {
  const client_map_type::iterator& client = find_client_by_file(event.ident);
  char buffer[kBUFFER_SIZE + 1];
  ssize_t read_size = 0;

  read_size = read(client->second.repository.get_fd(), buffer, kBUFFER_SIZE);

  if (read_size <= 0) { // todo: read 0 is an error?
    std::cerr << "Socket: read error occurred" << std::endl;
    _kernel.delete_read_event(event.ident);
    disconnect_client(event.ident);
    return;
  }

//  if (read_size == 0) { // todo: error?
//    close(client->second.repository.get_fd());
//  } else {
  buffer[read_size] = 0;
  client->second.response += buffer;
  if (Util::is_eof(event.ident)) {
    _kernel.delete_read_event(event.ident);
    _kernel.process_event(client->first, reinterpret_cast<void *>(ws::Socket::generate_response));
  }
//  }
}

void ws::Socket::write_data(struct kevent event) {
  const client_map_type::iterator& client = find_client_by_file(event.ident);
  const std::string& request_body = client->second.request.get_request_body();
  std::size_t& offset = client->second.write_offset;

  ssize_t write_size = write(client->second.repository.get_fd(), request_body.c_str() + offset, request_body.length() - offset);

  if (write_size == -1) {
    _kernel.delete_write_event(event.ident);
    disconnect_client(event.ident);
    return;
  }

  offset += write_size;

  if (offset == request_body.length()) {
    close(client->second.repository.get_fd());
    offset = 0;
    _kernel.delete_write_event(event.ident);
    _kernel.process_event(client->first, reinterpret_cast<void*>(ws::Socket::generate_response));
  }
}

void ws::Socket::generate_response(struct kevent event) {
  _response.generate(_client.find(event.ident)->second, event.ident);
  _kernel.add_write_event(event.ident, reinterpret_cast<void*>(ws::Socket::send_response));
}

void ws::Socket::send_response(struct kevent event) {
  client_value_type& client_data = _client.find(event.ident)->second;
  const std::string& response_data = client_data.response;
  std::string::size_type& offset = client_data.write_offset;

  ssize_t n;
  if ((n = write(event.ident, response_data.c_str() + offset, response_data.length() - offset)) == -1) {
    _kernel.delete_write_event(event.ident);
    disconnect_client(event.ident); // todo: close
    return;
  }

  offset += n;

  if (offset == response_data.length()) {
    _kernel.delete_write_event(event.ident);
    disconnect_client(event.ident);
  }
}
