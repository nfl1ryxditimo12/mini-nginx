#include "Socket.hpp"
#include "Repository.hpp"
#include "Request.hpp"
#include "Validator.hpp"

/* console test code */
#include <iostream> // 지울거임
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"
/* console test code */

ws::Socket::Socket(const ws::Configure& cls): _conf(&cls), _kernel(), _validator() {

  /* console test code */
  std::cout << YLW << "\n===================================================\n" << NC << std::endl;
  /* console test code */

  ws::Configure::listen_vec_type host = cls.get_host_list();

  for (size_t i = 0; i < host.size(); i++) {
    int                 socket_fd;
    struct sockaddr_in  addr_info;

    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
      throw; // require custom exception

    memset(&addr_info, 0, sizeof(addr_info));
    addr_info.sin_family = AF_INET;
    addr_info.sin_addr.s_addr = host[i].first;
    addr_info.sin_port = host[i].second;

    if (bind(socket_fd, (struct sockaddr*)&addr_info, sizeof(addr_info)) == -1) {
      std::cout << strerror(errno) << std::endl;
      throw; // reuqire custom exception
    }
    if (listen(socket_fd, 5) == -1)
      throw; // reuqire custom exception
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    _server.insert(server_map_type::value_type(socket_fd, host[i]));
    _kernel.kevent_ctl(socket_fd, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void*>(&Socket::connect_client));

    /* console test code */
    std::cout << socket_fd << ", " << ntohs(host[i].second) << std::endl;
    /* console test code */
  }

  /* console test code */
  std::cout << YLW << "\n===================================================\n" << NC << std::endl;
  /* console test code */
}

ws::Socket::~Socket() {}

void ws::Socket::connection() {
  while (1) {
    struct kevent* event_list = _kernel.kevent_wait();

    for (int i = 0; event_list[i].ident != 0 && i < 80; i++) {
      /* console test code */
      server_map_type::iterator server_it = _server.find(event_list[i].ident);
      std::cout << (server_it != _server.end() ? "[Server] " : "[Client] ") << event_list[i].ident << ", " << (event_list[i].filter == EVFILT_READ ? std::string(RED) + "READ" : (event_list[i].filter == EVFILT_WRITE ? std::string(GRN) + "WRITE" : "ELSE")) << NC << std::endl;
      /* console test code */

      // if (server_it == _server.end() && event_list[i].filter == EVFILT_READ && event_list[i].data == 0) {
      //   _kernel.kevent_ctl(event_list[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
      //   process_request(this, event_list[i]);
      // }
      // else {
        /* 이부분 kevent error시 예외처리 로직 들어가야함 */
        kevent_func func = reinterpret_cast<kevent_func>(event_list[i].udata);
        (*func)(this, event_list[i]);
      // }
    }
    delete event_list;
  }
}

/* Private function */

void ws::Socket::connect_client(ws::Socket* self, struct kevent event) {
  listen_type& listen = self->_server.find(event.ident)->second;
  int client_socket_fd;

  if ((client_socket_fd = accept(event.ident, NULL, NULL)) == -1)
    throw; // require custom exception
  fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
  self->_client.insert(client_map_type::value_type(client_socket_fd, client_value_type(ws::Request(listen), ws::Repository())));
  self->_kernel.kevent_ctl(client_socket_fd, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void*>(&Socket::recv_request));
}

void ws::Socket::recv_request(ws::Socket* self, struct kevent event) {
  char buffer[1024];
  int  n;

  if ((n = read(event.ident, buffer, sizeof(buffer))) == -1)
    throw; // require custom exception

  /* console test code */
  // std::cout << n << ", " << event.data << std::endl;
  /* console test code */

  if (n > 0)
    self->_client.find(event.ident)->second.first.parse_request_message(buffer);
  if (event.data == n) {
    self->_kernel.kevent_ctl(event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    self->_kernel.kevent_ctl(event.ident, EVFILT_USER, EV_ADD, NOTE_TRIGGER, 0, reinterpret_cast<void*>(&Socket::build_request));
  }
}

void ws::Socket::build_request(ws::Socket* self, struct kevent event) {
  client_value_type& client_value = self->_client.find(event.ident)->second;
  ws::Request& request = client_value.first;
  ws::Repository& repository = client_value.second;
  const ws::Server* curr_server = self->_conf->find_server(request.get_listen(), request.get_server_name());
  repository(curr_server, request);
  self->_validator(client_value);

  self->_kernel.kevent_ctl(event.ident, EVFILT_USER, EV_ADD, NOTE_TRIGGER, 0, reinterpret_cast<void*>(&Socket::process_request));
}

void ws::Socket::process_request(ws::Socket* self, struct kevent event) {
  /*
    business logic
    비즈니스 로직 처리 후 어떤 식으로 response data 저장할 지 생각해 봐야 함
  */

  self->_kernel.kevent_ctl(event.ident, EVFILT_USER, EV_DELETE, 0, 0, NULL);
  self->_kernel.kevent_ctl(event.ident, EVFILT_WRITE, EV_ADD, 0, 0, reinterpret_cast<void*>(&Socket::send_response));
}

void ws::Socket::send_response(ws::Socket* self, struct kevent event) {
  int n;
  std::string body = "hello world " + std::to_string(event.ident);
  std::string response = std::string("HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\nContent-Type: text\r\nDate: Mon, 20 Jun 2022 02:59:03 GMT\r\nETag: \"62afd0a1-267\"\r\nLast-Modified: Mon, 20 Jun 2022 01:42:57 GMT\r\nServer: webserv\r\n\r\n") + body;
  // struct stat test;
  // int ret = fstat(event.ident, &test);
  // std::cout << ret << ", " << test.st_size << ", " << (S_ISSOCK(test.st_mode) ? "SOCKET" : "ELSE") << std::endl;
  n = write(event.ident, response.c_str(), response.size());
  if (event.data == n) {
    self->_client.erase(event.ident);
    close(event.ident);
  }
}
