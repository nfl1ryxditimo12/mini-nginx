#include "Socket.hpp"

/*
  kevent udata 구조체 초기화 이슈
  주소를 넘겨주고, 내부적으로 초기화가 되나?
  1. 객체를 넘겨줘 보자.
  2. 다른 타입으로도 해보자.
  3. 다른 임시 구조체를 생성해서 한번 해보자
  4. 형변환 시 초기화 되는 이유를 알아보면 좋을 것 같다.
*/

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

  _kernel.resize_event_list(10);
}

ws::Socket::Socket(const ws::Configure& cls): _kernel() {
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

      if (bind(socket_fd, (struct sockaddr*)&addr_info, sizeof(addr_info)) == -1)
        throw; // reuqire custom exception
      if (listen(socket_fd, 5) == -1)
        throw; // reuqire custom exception
      fcntl(socket_fd, F_SETFL, O_NONBLOCK);
      _server.insert(server_type::value_type(socket_fd, listen_block[j]));
      _kernel.kevent_ctl(socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(&Socket::connect_client));
    }
  }

  _kernel.resize_event_list(_server.size());
}

ws::Socket::~Socket() {}

void ws::Socket::request_handler() {
  while (1) {
    struct kevent event_list = _kernel.kevent_wait();

    kevent_func func = reinterpret_cast<kevent_func>(event_list.udata);
    (*func)(this, event_list);
    // if (event_list.udata != NULL) {
    //   info->event = &event_list;
    //   if (info->func != NULL) {
    //     (*info->func)(info);
    //   }
    // }  

  //   for (size_t i = 0; i < event_list.size(); i++) {
  //     struct kevent* curr_event = &event_list[i];
  //     kevent_data* info = static_cast<kevent_data*>(curr_event->udata);

  //     if (curr_event->udata != NULL) {
  //       info->event = curr_event;
  //       if (info->func != NULL) {
  //         (*info->func)(info);
  //       }
  //     }
  //   }
  }
}

/* Private function */

void ws::Socket::connect_client(ws::Socket* self, struct kevent event) {
  server_type::iterator it = self->_server.find(event.ident);
  int client_socket_fd;

  if ((client_socket_fd = accept(event.ident, NULL, NULL)) == -1)
    throw; // require custom exception
  fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
  self->_client.insert(client_type::value_type(client_socket_fd, ws::Request(it->second)));
  self->_kernel.kevent_ctl(client_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(&Socket::parse_request));
}

#include <iostream>

void ws::Socket::parse_request(ws::Socket* self, struct kevent event) {
  char buffer[1024];
  int  n;

  if ((n = read(event.ident, buffer, sizeof(buffer))) == -1)
    throw; // require custom exception

  std::cout << n << ", " << event.data << std::endl;

  if (n > 0)
    self->_client.find(event.ident)->second.parse_request_message(buffer);
  if (event.data == n) {
    /* validator */
    /* business logic */
    self->_kernel.kevent_ctl(event.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(&Socket::send_response));
  }
}

void ws::Socket::send_response(ws::Socket* self, struct kevent event) {
  std::string body = "hello world " + std::to_string(event.ident);
  std::string response = std::string("HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\nContent-Type: text\r\nDate: Mon, 20 Jun 2022 02:59:03 GMT\r\nETag: \"62afd0a1-267\"\r\nLast-Modified: Mon, 20 Jun 2022 01:42:57 GMT\r\nServer: webserv\r\n\r\n") + body;
  write(event.ident, response.c_str(), response.size());
  self->_client.erase(event.ident);
  close(event.ident);
}
