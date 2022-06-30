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
    kevent_data info;
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
      info = init_kevent_udata(&Socket::connect_client, NULL);
      _server.insert(std::pair<int, struct sockaddr_in>(socket_fd, addr_info));
      std::string test = "hello world";
      _kernel.kevent_ctl(socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, static_cast<void*>(&test));
  }

  _kernel.resize_event_list(10);
}

ws::Socket::Socket(const ws::Configure& cls): _kernel() {
  std::vector<ws::Server> server_config = cls.get_server();

  for (size_t i = 0; i < server_config.size(); i++) {
    ws::Server::listen_type listen_block = server_config[i].get_listen();

    for (size_t j = 0; j < server_config[i].get_listen().size(); j++) {
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
      _server.insert(std::pair<int, struct sockaddr_in>(socket_fd, addr_info));
      // _kernel.add_change_list(socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, static_cast<void*>(init_kevent_udata(&Socket::connect_client, NULL)));
    }
  }

  _kernel.resize_event_list(_server.size());
}

ws::Socket::~Socket() {}

void ws::Socket::request_handler() {
  while (1) {
    struct kevent event_list = _kernel.kevent_wait();

    kevent_data* info = static_cast<kevent_data*>(event_list.udata);

    if (event_list.udata != NULL) {
      info->event = &event_list;
      if (info->func != NULL) {
        (*info->func)(info);
      }
    }  

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

/* 마지막 인자로 ws::ResponseMessage = NULL 받아와야함 #추후수정 */
ws::Socket::kevent_data ws::Socket::init_kevent_udata(kevent_func func, ws::RequestMessage* request) {
  kevent_data info;

  info.self = NULL;
  info.event = NULL;
  info.func = func;
  info.request = request;
  // info.response = response;
  return info;
}

void ws::Socket::connect_client(kevent_data* info) {
  kevent_data new_info = info->self->init_kevent_udata(&Socket::parse_request, NULL);
  ws::RequestMessage request;
  int client_socket_fd;

  if ((client_socket_fd = accept(info->event->ident, NULL, NULL)) == -1)
    throw; // require custom exception
  fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
  info->self->_kernel.kevent_ctl(client_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, static_cast<void*>(&new_info));
  info->self->_client.insert(client_type::value_type(client_socket_fd, request.get_RequestMessage()));
}

void ws::Socket::parse_request(kevent_data* info) {
  char buffer[1024];
  int  n;

  if ((n = read(info->event->ident, buffer, sizeof(buffer))) == -1)
    throw; // require custom exception

  if (n > 0)
    info->self->_client.find(info->event->ident)->second->parse_request_message(buffer, n);
  if (info->event->data == n) {
    kevent_data new_info = info->self->init_kevent_udata(&Socket::send_response, NULL);

    info->self->_kernel.kevent_ctl(info->event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, static_cast<void*>(&new_info));
  }
}

void ws::Socket::send_response(kevent_data* info) {
  std::string body = "hello world " + std::to_string(info->event->ident);
  std::string response = std::string("HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nConnection: keep-alive\r\nContent-Length: ") + std::to_string(body.length()) + std::string("\r\nContent-Type: text\r\nDate: Mon, 20 Jun 2022 02:59:03 GMT\r\nETag: \"62afd0a1-267\"\r\nLast-Modified: Mon, 20 Jun 2022 01:42:57 GMT\r\nServer: webserv\r\n\r\n") + body;
  write(info->event->ident, response.c_str(), response.size());
  info->self->_client.erase(info->event->ident);
  close(info->event->ident);
}
