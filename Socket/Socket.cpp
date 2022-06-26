#include "Socket.hpp"

ws::Socket::Socket(const ws::Configure& cls): _kernel() {
  std::vector<ws::Server> server_config = cls.get_server();

  for (int i = 0; i < server_config.size(); i++) {
    ws::Server::listen_type listen_block = server_config[i].get_listen();

    for (int j = 0; j < server_config[i].get_listen().size(); j++) {
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
      _kernel.add_change_list(socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void*)&Socket::connect_client);
    }
  }

  _kernel.resize_event_list(_server.size());
}

ws::Socket::~Socket() {}

void ws::Socket::request_handler() {
  int new_events = 0;

  while (1) {
    ws::Kernel::kevent_vector event_list = _kernel.event_handler();

    for (int i = 0; i < event_list.size(); i++) {
      struct kevent* curr_event = &event_list[i];

      if (curr_event->udata != NULL) {
        kevent_data* info = reinterpret_cast<kevent_data*>(curr_event->udata);
        if (info->func != NULL) {
          info->func(info);
        }
      }
    }
  }
}

/* Private function */

/* 마지막 인자로 ws::ResponseMessage = NULL 받아와야함 #추후수정 */
ws::Socket::kevent_data ws::Socket::init_kevent_udata(void* func = NULL, ws::RequestMessage* request = NULL) {
  kevent_data info;

  info.self = NULL;
  info.event = NULL;
  info.func = (func != NULL ? reinterpret_cast<kevent_func>(func) : NULL);
  info.request = request;
  // info.response = response;
  return info;
}

void ws::Socket::connect_client(kevent_data* info) {
  int client_socket_fd;

  if ((client_socket_fd = accept(info->event->ident, NULL, NULL)) == -1)
    throw; // require custom exception
  fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
  info->self->_kernel.add_change_list(client_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, \
                          (void*)&info->self->init_kevent_udata((void*)&Socket::parse_request));
  info->self->_client.insert(client_type::value_type(client_socket_fd, &ws::RequestMessage()));
}

void ws::Socket::parse_request(kevent_data* info) {
  char buffer[1024];
  int  n;

  if ((n = read(info->event->ident, buffer, sizeof(buffer))) == -1)
    throw; // require custom exception

  if (n > 0)
    info->self->_client.find(info->event->ident)->second->parse_request_message(buffer, n);
  if (info->event->data == n)
    info->self->_kernel.add_change_list(info->event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, \
                          (void*)&info->self->init_kevent_udata((void*)&Socket::send_response));
}

void ws::Socket::send_response(kevent_data* info) {
  
}