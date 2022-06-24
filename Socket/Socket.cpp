#include "Socket.hpp"

ws::Socket::Socket(const ws::Configure& cls) {
  std::vector<ws::Server> server_config = cls.get_server();

  _kq = kqueue();
  if (_kq == -1)
    throw; // require custom exception

  for (int i = 0; i < server_config.size(); i++) {
    ws::Server::listen_type listen = server_config[i].get_listen();

    for (int j = 0; j < server_config[i].get_listen().size(); j++){
      std::pair<int, struct sockaddr_in> server_pair;
      if ((server_pair.first = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        throw; // require custom exception

      memset(&server_pair.second, 0, sizeof(server_pair.second));
      server_pair.second.sin_family = AF_INET;
      server_pair.second.sin_addr.s_addr = listen[j].first;
      server_pair.second.sin_port = listen[j].second;
      _server.insert(server_pair);
    }
  }

  for (server_type::iterator it = _server.begin(); it != _server.end(); ++it) {
    if (bind(it->first, (struct sockaddr*)&it->second, sizeof(it->second)) == -1)
      throw; // reuqire custom exception
    if (listen(it->first, 5) == -1)
      throw; // reuqire custom exception
    fcntl(it->first, F_SETFL, O_NONBLOCK);
    add_new_kevent(_change_list, it->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  }

  _event_list.resize(_server.size() * 8);
}

ws::Socket::~Socket() {}

void ws::Socket::kernel_handler() {
  int new_events = 0;

  while (1) {
    _event_list.clear();
    new_events = kevent(_kq, &_change_list[0], _change_list.size(), &_event_list[0], _event_list.size(), NULL);
    if (new_events == -1)
      throw; // require custom exception
    _change_list.clear();

    for (int i = 0; i < new_events; i++) {
      struct kevent* curr_event = &_event_list[i];
      server_type::iterator server_iter = _server.find(curr_event->ident);
      client_type::iterator client_iter = _client.find(curr_event->ident);

      if (curr_event->filter == EVFILT_READ) {
        if (server_iter != _server.end()) {
          int client_socket_fd;

          if ((client_socket_fd = accept(curr_event->ident, NULL, NULL)) == -1)
            throw; // require custom exception
          fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
          add_new_kevent(_change_list, client_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
          _client.insert(client_type::value_type(client_socket_fd, &Socket::get_server_info));
        }
      }
    }
  }
}

/* Private function */

void ws::Socket::add_new_kevent(kevent_vector& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

ws::Socket::server_type::iterator ws::Socket::get_server_info(int server_fd) {
  return _server.find(server_fd);
}