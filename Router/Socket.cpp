#include "Socket.hpp"

#include "Response.hpp"
#include "Validator.hpp"

ws::Validator ws::Socket::_validator;
ws::Response ws::Socket::_response;
const std::size_t ws::Socket::kBUFFER_SIZE = 1024 * 1024;

/*
  메모리를 많이 사용하고 CPU를 적게 사용할 지
  반대로 사용할 지 trade-off 케이스를 생각해 보자
  보통 서버에선 전자로 많이 사용한다.
*/

/* console test code */
#include <iostream>
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"
/* console test code */

ws::Socket::Socket(const ws::Configure& cls): _conf(cls), _kernel() {
  _response.set_kernel(&_kernel);
  ws::Configure::listen_vec_type host = cls.get_host_list();

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
    _kernel.kevent_ctl(socket_fd, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void*>(&Socket::connect_client));
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

void ws::Socket::connection() {
  size_t event_size = _server.size() * 8;
  struct kevent event_list[event_size];
  int new_event;

  while (true) {
    new_event = _kernel.kevent_wait(event_list, event_size);

    for (int i = 0; i < new_event; i++) {
      kevent_func func = reinterpret_cast<kevent_func>(event_list[i].udata);
      (*func)(this, event_list[i]);
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

void ws::Socket::connect_client(ws::Socket* self, struct kevent event) {
  listen_type& listen = self->_server.find(event.ident)->second;
  int client_socket_fd;

  if ((client_socket_fd = accept(event.ident, NULL, NULL)) == -1)
    return;

  fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
  self->init_client(client_socket_fd, listen);
  self->_kernel.kevent_ctl(client_socket_fd, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void*>(&Socket::recv_request)); // todo
}

/*
  eof가 아닌데 buffer size가 0인 경우 타임아웃 처리를 해야할 듯
*/
void ws::Socket::recv_request(ws::Socket* self, struct kevent event) {
  client_value_type& client_data = self->_client.find(event.ident)->second;
  char buffer[kBUFFER_SIZE + 1];

  ssize_t read_size;
  read_size = read(event.ident, buffer, std::min(static_cast<long>(kBUFFER_SIZE), event.data));

  if (read_size == -1) {
    self->_kernel.kevent_ctl(event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    self->disconnect_client(event.ident);
    return;
  }

  buffer[read_size] = 0;
  // std::cout << "input" << std::endl;
  // std::cout << buffer << std::endl; // todo;
  // std::cout << "input end" << std::endl;

  if (client_data.request.eof() && read_size > 0) // todo session
    client_data.request.clear();

  if (read_size > 0) { // todo: test print
//    std::cout << YLW << "\n== Request ======================================\n" << NC << std::endl;
//    std::cout << buffer << std::endl;
//    std::cout << RED << "\n== Parsing ======================================\n" << NC << std::endl;
    client_data.status = client_data.request.parse_request_message(self->_conf, buffer, client_data.repository);
  }

//  todo
  if (read_size == 0) {
    self->_kernel.kevent_ctl(event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    self->disconnect_client(event.ident);
//     operation timeout
  }

  /*
    Request 클래스에서 모든 데이터를 다 읽었다면 eof == true로 설정된다.
    이 후 오류가 있다면 바로 process_request 함수로 가게 설정해준다.
    Repository 클래스를 Request 클래스에서 request header 파싱 후 초기화 해줄 지 고민해봐야 함
  */
  if (client_data.request.eof() || client_data.status || !read_size) {
    /* EV_DELETE flags는 필요 없을듯 keep-alive 생각 */

//    client_data.request.test(); // todo: test print
//    std::cout << YLW << "\n=================================================\n" << NC << std::endl;
    self->_kernel.kevent_ctl(event.ident, EVFILT_USER, EV_ADD | EV_ONESHOT, NOTE_TRIGGER, 0, reinterpret_cast<void*>(&Socket::process_request));
    self->_kernel.kevent_ctl(event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  }
}

void ws::Socket::process_request(ws::Socket* self, struct kevent event) {
  client_value_type& client_data = self->_client.find(event.ident)->second;

  if (!client_data.status)
    _validator(client_data);

  client_data.repository.set_repository(client_data.status);
  client_data.status = client_data.repository.get_status();
  client_data.fatal = client_data.repository.is_fatal();

  if (client_data.fatal) {
    self->disconnect_client(event.ident);
    return;
  }

  _response.process(self, client_data, event.ident);

  /*
    EVFILT_USER를 사용하는 경우 EV_ONESHOT flag 사용으로
    한번만 kevent의 change_list에 넣는 방식도 생각해 볼만 하다.
  */
//  self->_kernel.kevent_ctl(event.ident, EVFILT_WRITE, EV_ADD, 0, 0, reinterpret_cast<void*>(&Socket::send_response));
}

void ws::Socket::send_response(ws::Socket *self, struct kevent event) {
  client_value_type& client_data = self->_client.find(event.ident)->second;
  const std::string& response_data = client_data.response;
  std::string::size_type& offset = client_data.write_offset;

  ssize_t n;
  if ((n = write(event.ident, response_data.c_str() + offset, response_data.length() - offset)) == -1) {
    self->_kernel.kevent_ctl(event.ident, EVFILT_USER, EV_DELETE, 0, 0, NULL);
    self->disconnect_client(event.ident); // todo: close
    return;
  }

  offset += n;

  if (offset == response_data.length()) {
    self->_kernel.kevent_ctl(event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    self->disconnect_client(event.ident);
//    struct timespec limit;
//    limit.tv_sec = 2;
//    limit.tv_nsec = 0;
//
//    client_data.request.clear();
//    client_data.repository.clear();
//    client_data.response.clear();
//    self->_kernel.kevent_ctl(
//      event.ident,
//      EVFILT_READ,
//      EV_ADD,
//      0,
//      0,
//      reinterpret_cast<void*>(ws::Socket::recv_request),
//      &limit
//    );
    // todo
  }
}
//  int fd = open("/goinfre/jaham/webserv/test_create.html", O_WRONLY | O_TRUNC | O_CREAT, 0666);
  // int fd = open("/goinfre/jaham/webserv/test.html", O_RDONLY);

void ws::Socket::read_data(ws::Socket* self, struct kevent event) {
  const client_value_type& client = self->_client.find(event.ident)->second;
  char buffer[kBUFFER_SIZE + 1];
  ssize_t read_size = 0;

  read_size = read(client.repository.get_fd(), buffer, kBUFFER_SIZE);

  if (read_size < 0) {
    self->_kernel.kevent_ctl(event.ident, EVFILT_USER, EV_DELETE, 0, 0, NULL);
    self->disconnect_client(event.ident);
    return;
  }

  if (read_size == 0) {
    close(client.repository.get_fd());
    self->_kernel.kevent_ctl(event.ident, EVFILT_USER, EV_DELETE, 0, 0, NULL);
    self->_kernel.kevent_ctl(
      event.ident,
      EVFILT_USER,
      EV_ADD | EV_ONESHOT,
      NOTE_TRIGGER,
      0,
      reinterpret_cast<void *>(ws::Socket::generate_response)
    );
  } else {
    buffer[read_size] = 0;
    self->_client.find(event.ident)->second.response += buffer;
  }
}

void ws::Socket::write_data(ws::Socket *self, struct kevent event) {
  client_value_type& client = self->_client.find(event.ident)->second;
  const std::string& request_body = client.request.get_request_body();
  std::size_t& offset = client.write_offset;
  ssize_t write_size = 0;

  write_size = write(client.repository.get_fd(), request_body.c_str() + offset, request_body.length() - offset);

  if (write_size == -1) {
    self->_kernel.kevent_ctl(event.ident, EVFILT_USER, EV_DELETE, 0, 0, NULL);
    self->disconnect_client(event.ident);
    return;
  }

  offset += write_size;

  if (offset == request_body.length()) {
    close(client.repository.get_fd());
    offset = 0;
    self->_kernel.kevent_ctl(event.ident, EVFILT_USER, EV_DELETE, 0, 0, NULL);
    self->_kernel.kevent_ctl(
      event.ident,
      EVFILT_USER,
      EV_ADD | EV_ONESHOT,
      NOTE_TRIGGER,
      0,
      reinterpret_cast<void*>(ws::Socket::generate_response)
    );
  }
}

void ws::Socket::generate_response(ws::Socket *self, struct kevent event) {
  struct timespec limit;
  limit.tv_sec = 2;
  limit.tv_nsec = 0;

  _response.generate(self, self->_client.find(event.ident)->second, event.ident);
  self->_kernel.kevent_ctl(
    event.ident,
    EVFILT_WRITE,
    EV_ADD,
    0,
    0,
    reinterpret_cast<void*>(ws::Socket::send_response)
  );
}
