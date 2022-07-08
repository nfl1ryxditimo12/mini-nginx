#include "Response.hpp"
#include "Validator.hpp"
#include "Socket.hpp"


ws::Validator ws::Socket::_validator;
ws::Response ws::Socket::_response;

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

ws::Socket::Socket(const ws::Configure& cls): _conf(&cls), _kernel() {
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
    if (listen(socket_fd, 5) == -1)
      exit_socket();
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    _server.insert(server_map_type::value_type(socket_fd, host[i]));
    _kernel.kevent_ctl(socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(&Socket::connect_client));
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

  while (1) {
    new_event = _kernel.kevent_wait(event_list, event_size);

    for (int i = 0; i < new_event; i++) {
      kevent_func func = reinterpret_cast<kevent_func>(event_list[i].udata);
      (*func)(this, event_list[i]);
    }
  }
}

/* Private function */

void ws::Socket::init_client(int fd, listen_type listen) {
  client_value_type* client_data = new client_value_type;

  client_data->fatal = false;
  client_data->status = 0;
  client_data->repository = new ws::Repository(client_data->fatal, client_data->status);
  client_data->request = new ws::Request(listen);
  _client.insert(client_map_type::value_type(fd, client_data));
}

void ws::Socket::disconnect_client(int fd) {
  client_map_type::iterator client_iter = _client.find(fd);

  if (client_iter == _client.end())
    return;

  client_value_type* client_data = client_iter->second;

  delete client_data->request;
  delete client_data->repository;
  delete client_data;

  _client.erase(client_iter);
  close(fd);
}

void ws::Socket::exit_socket() {
  for (client_map_type::iterator it = _client.begin(); it != _client.end(); ++it) {
    client_value_type* client_data = it->second;

    delete client_data->request;
    delete client_data->repository;
    delete client_data;
  }
  exit(1);
}

void ws::Socket::connect_client(ws::Socket* self, struct kevent event) {
  listen_type& listen = self->_server.find(event.ident)->second;
  int client_socket_fd;

  if ((client_socket_fd = accept(event.ident, NULL, NULL)) == -1)
    self->exit_socket();
  fcntl(client_socket_fd, F_SETFL, O_NONBLOCK);
  self->init_client(client_socket_fd, listen);
  self->_kernel.kevent_ctl(client_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(&Socket::recv_request));
}

/*
  eof가 아닌데 buffer size가 0인 경우 타임아웃 처리를 해야할 듯
*/
void ws::Socket::recv_request(ws::Socket* self, struct kevent event) {
  client_value_type* client_data = self->_client.find(event.ident)->second;
  std::string str_buffer;
  char buffer[1024 * 1024 + 1];
  int  n = 0;

  ssize_t read_size;
  while (n < event.data) {
    if ((read_size = read(event.ident, buffer, std::min(static_cast<int>(event.data), 1024 * 1024 - n))) == -1)
      self->exit_socket();
    buffer[read_size] = 0;
    str_buffer += buffer;
    n += read_size;
  }

  // if ((n = read(event.ident, buffer, std::min(sizeof(buffer), static_cast<size_t>(event.data) - 1))) == -1)
  //   self->exit_socket();

  if (client_data->request->eof() && n > 0)
    client_data->request->clear();

  /*
    과연 if (n > 0) 이라는 조건이 필요할까?
    Request 클래스 내부적으로 판단할 지 생각해 보자.
  */
  if (!client_data->request->eof() && n > 0) {
    std::cout << YLW << "\n== Request ======================================\n" << NC << std::endl;
    std::cout << buffer << std::endl;
    std::cout << RED << "\n== Parsing ======================================\n" << NC << std::endl;
    client_data->status = client_data->request->parse_request_message(self->_conf, buffer);
  }

  /*
    Request 클래스에서 모든 데이터를 다 읽었다면 eof == true로 설정된다.
    이 후 오류가 있다면 바로 process_request 함수로 가게 설정해준다.
    Repository 클래스를 Request 클래스에서 request header 파싱 후 초기화 해줄 지 고민해봐야 함
  */
  if (client_data->request->eof() || client_data->status || !n) {
    /* EV_DELETE flags는 필요 없을듯 keep-alive 생각 */

    client_data->request->test();
    std::cout << YLW << "\n=================================================\n" << NC << std::endl;
    self->_kernel.kevent_ctl(event.ident, EVFILT_USER, EV_ADD | EV_ONESHOT, NOTE_TRIGGER, 0, reinterpret_cast<void*>(&Socket::process_request));
    self->_kernel.kevent_ctl(event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);

  }
}

void ws::Socket::process_request(ws::Socket* self, struct kevent event) {
  client_value_type* client_data = self->_client.find(event.ident)->second;
  ws::Repository& repository = *client_data->repository;
  (void) repository; // todo
  if (!client_data->status)
    _validator(*client_data);

  _response.process(self, *client_data, event.ident);

  /*
    EVFILT_USER를 사용하는 경우 EV_ONESHOT flag 사용으로
    한번만 kevent의 change_list에 넣는 방식도 생각해 볼만 하다.
  */
//  self->_kernel.kevent_ctl(event.ident, EVFILT_WRITE, EV_ADD, 0, 0, reinterpret_cast<void*>(&Socket::send_response));
}

void ws::Socket::send_response(ws::Socket *self, struct kevent event) {
  client_value_type& client_data = *(self->_client.find(event.ident)->second);
  const std::string& response_data = client_data.response.first;
  std::string::size_type& pos = client_data.response.second;

  ssize_t n;
  if ((n = write(event.ident, response_data.c_str() + pos, response_data.length() - pos)) == -1)
    self->exit_socket();

  pos += n;

  if (pos == response_data.length()) {
    self->_kernel.kevent_ctl(event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    self->disconnect_client(event.ident);
  }
}
  int fd = open("/goinfre/jaham/webserv/test_create.html", O_WRONLY | O_TRUNC | O_CREAT, 0666);

void ws::Socket::read_data(ws::Socket* self, struct kevent event) {
  char buffer[1024 * 1024 + 1];
  ssize_t read_size = 0;

  read_size = read(fd, buffer, 1024 * 1024);

  if (read_size < 0)
    self->exit_socket();

  if (read_size == 0) {
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
    self->_client.find(event.ident)->second->response.first += buffer;
  }
}

void ws::Socket::write_data(ws::Socket *self, struct kevent event) {
  const std::string& request_body = self->_client.find(event.ident)->second->request->get_request_body();
  size_t pos = 0;
  ssize_t write_size = 0;

  while (true) {
    if ((write_size = write(fd, request_body.c_str() + pos, request_body.length() - pos)) == -1)
      self->exit_socket();

    pos += write_size;

    if (pos == request_body.length())
      break;
  }

  self->_kernel.kevent_ctl(
    event.ident,
    EVFILT_USER,
    EV_ADD | EV_ONESHOT,
    NOTE_TRIGGER,
    0,
    reinterpret_cast<void*>(ws::Socket::generate_response)
  );
}

void ws::Socket::generate_response(ws::Socket *self, struct kevent event) {
  _response.generate(self, *(self->_client.find(event.ident)->second), event.ident);
  self->_kernel.kevent_ctl(
    event.ident,
    EVFILT_WRITE,
    EV_ADD,
    0,
    0,
    reinterpret_cast<void*>(ws::Socket::send_response)
  );
}

// todo: original send response
//void ws::Socket::send_response(ws::Socket* self, struct kevent event) {
//  // client_value_type* client_data = self->_client.find(event.ident)->second;
//  int n;
//  const std::string& response = self->_client.find(event.ident)->second->response;
//
//  if ((n = write(event.ident, response.c_str(), response.size())) == -1)
//    self->exit_socket();
//
//  /*
//    keep-alive 방식으로 납둘지 close 해버릴지 고민해봐야함
//    세션같은 경우 keep-alive가 좋을 수도 있다.
//
//    EVFILT_WRITE인 경우 event.data의 값이 커널에 할당 가능한 buffer size 일텐데 이게 정확한 방법인지 생각해봐야 한다.
//
//    keep-alive인 경우 client_data 구조체 초기화 후, EVFILT_WRITE - EV_DELETE 해주면 끗
//  */
//  std::cout << n << ", " << event.data << std::endl;
//  if (n <= event.data) {
//    self->_kernel.kevent_ctl(event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
//    self->disconnect_client(event.ident);
//  }
//}
