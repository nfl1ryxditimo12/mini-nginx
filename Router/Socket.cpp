#include "Socket.hpp"

/* console test code */
#include <iostream> // 지울거임
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"
/* console test code */

/* test code */
ws::Socket::Socket(int port): _kernel() {
  std::vector<struct kevent> change_list;

  for (int i = 0; i < 10; i++) {
    int socket_fd;
    struct sockaddr_in addr_info;

    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        throw; // require custom exception

      memset(&addr_info, 0, sizeof(addr_info));
      addr_info.sin_family = AF_INET;
      addr_info.sin_addr.s_addr = htonl(INADDR_ANY);
      addr_info.sin_port = htons(port + i);

      if (bind(socket_fd, (struct sockaddr*)&addr_info, sizeof(addr_info)) == -1)
        throw; // reuqire custom exception
      if (listen(socket_fd, 5) == -1)
        throw; // reuqire custom exception
      fcntl(socket_fd, F_SETFL, O_NONBLOCK);
      // info = init_kevent_udata(&Socket::connect_client, NULL);
      // _server.insert(std::pair<int, struct sockaddr_in>(socket_fd, addr_info));
  }
}
/* test code */

ws::Socket::Socket(const ws::Configure& cls): _kernel() {

  /* console test code */
  std::cout << YLW << "\n===================================================\n" << NC << std::endl;
  /* console test code */

  ws::Configure::server_vec_type server_block = cls.get_server_vec();

  for (size_t i = 0; i < server_block.size(); i++) {
    ws::Configure::listen_vec_type listen_block = server_block[i].get_listen_vec();

    for (size_t j = 0; j < listen_block.size(); j++) {
      int                 socket_fd;
      struct sockaddr_in  addr_info;

      if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        throw; // require custom exception

      memset(&addr_info, 0, sizeof(addr_info));
      addr_info.sin_family = AF_INET;
      addr_info.sin_addr.s_addr = listen_block[j].first;
      addr_info.sin_port = listen_block[j].second;

      if (bind(socket_fd, (struct sockaddr*)&addr_info, sizeof(addr_info)) == -1) {
        std::cout << strerror(errno) << std::endl;
        throw; // reuqire custom exception
      }
      if (listen(socket_fd, 5) == -1)
        throw; // reuqire custom exception
      fcntl(socket_fd, F_SETFL, O_NONBLOCK);
      _server.insert(server_map_type::value_type(socket_fd, listen_block[j]));
      _kernel.kevent_ctl(socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(&Socket::connect_client));

      /* console test code */
      std::cout << socket_fd << ", " << ntohs(listen_block[j].second) << std::endl;
      /* console test code */
    }
  }

  /* console test code */
  std::cout << YLW << "\n===================================================\n" << NC << std::endl;
  /* console test code */
}

ws::Socket::~Socket() {}

void ws::Socket::connection() {
  while (1) {
    struct kevent event_list = _kernel.kevent_wait();

    /* console test code */
    server_map_type::iterator server_it = _server.find(event_list.ident);
    std::cout << (server_it != _server.end() ? "[Server] " : "[Client] ") << event_list.ident << ", " << (event_list.filter == EVFILT_READ ? std::string(RED) + "READ" : std::string(GRN) + "WRITE") << NC << std::endl;
    /* console test code */

    /* 이부분 kevent error시 예외처리 로직 들어가야함 */
    kevent_func func = reinterpret_cast<kevent_func>(event_list.udata);
    (*func)(this, event_list);
  }
}

/* Private function */

void ws::Socket::connect_client(ws::Socket* self, struct kevent event) {
  server_map_type::iterator it = self->_server.find(event.ident);
  int client_socket_fd;

  if ((client_socket_fd = accept(event.ident, NULL, NULL)) == -1)
    throw; // require custom exception
  fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
  self->_client.insert(client_map_type::value_type(client_socket_fd, ws::Request(it->second)));
  self->_kernel.kevent_ctl(client_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(&Socket::recv_request));
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
    self->_client.find(event.ident)->second.parse_request_message(buffer);
  if (event.data == n) {
    /* validator */
    /*
      business logic
      비즈니스 로직 처리 후 어떤 식으로 response data 저장할 지 생각해 봐야 함
    */
    self->_kernel.kevent_ctl(event.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(&Socket::send_response));
  }
}

void ws::Socket::send_response(ws::Socket* self, struct kevent event) {
  std::string body = "hello world " + std::to_string(event.ident);
  std::string response = std::string("HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\nContent-Type: text\r\nDate: Mon, 20 Jun 2022 02:59:03 GMT\r\nETag: \"62afd0a1-267\"\r\nLast-Modified: Mon, 20 Jun 2022 01:42:57 GMT\r\nServer: webserv\r\n\r\n") + body;
  write(event.ident, response.c_str(), response.size());
  self->_client.erase(event.ident);
  close(event.ident);
  // self->_kernel.kevent_ctl(event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
}
