#include "Kernel.hpp"

#include <cstdio>


extern bool webserv_fatal;

ws::Kernel::Kernel() throw() {
  try {
    _kq = kqueue();
    if (_kq == -1)
      throw; // require custom exception
  } catch (...) {
    webserv_fatal = true;
  }
}

ws::Kernel::~Kernel() {
  close(_kq);
  _change_list.clear();
}

/*
  커널에서 이벤트 핸들링 하고 싶은 kevent 구조체 등록
*/

// todo: 이제 불필요한 함수 삭제 예정
void  ws::Kernel::kevent_ctl() {
  if (!_change_list.size())
    return;

  if (kevent(_kq, &_change_list[0], _change_list.size(), NULL, 0, NULL) == -1) {
    perror("kevent");
    throw; // require custom exception
  }
  _change_list.clear();
}
/*
  커널에서 발생한 이벤트 리턴해주는 함수
*/

// todo: 네이밍 변경
int ws::Kernel::kevent_wait(struct kevent* event_list, size_t event_size) {
  int new_event;

  std::memset(event_list, 0, sizeof(struct kevent) * event_size);
  if ((new_event = kevent(_kq, &_change_list[0], _change_list.size(), event_list, event_size, NULL)) == -1)
    throw; // require custom exception
  _change_list.clear();
  return new_event;
}

void  ws::Kernel::add_signal_event(int ident, void* udata, uint16_t flags, uint32_t fflags, intptr_t data) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_SIGNAL, EV_ADD | flags, fflags, data, udata);
  _change_list.push_back(event);
}

void ws::Kernel::add_read_event(int ident, void *udata, uint16_t flags, uint32_t fflags, intptr_t data) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_READ, EV_ADD | flags, fflags, data, udata);
  _change_list.push_back(event);
}

void ws::Kernel::add_write_event(int ident, void *udata, uint16_t flags, uint32_t fflags, intptr_t data) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_WRITE, EV_ADD | flags, fflags, data, udata);
  _change_list.push_back(event);
}

void ws::Kernel::add_process_event(int ident, void *udata, uint16_t flags, uint32_t fflags, intptr_t data) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_PROC, EV_ADD | EV_ONESHOT | flags, fflags, data, udata);
  _change_list.push_back(event);
}

void ws::Kernel::add_user_event(int ident, void *udata, uint16_t flags, uint32_t fflags, intptr_t data) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_USER, EV_ADD | flags, NOTE_TRIGGER | fflags, data, udata);
  _change_list.push_back(event);
}

void ws::Kernel::delete_read_event(int ident) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  _change_list.push_back(event);
}

void ws::Kernel::delete_write_event(int ident) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  _change_list.push_back(event);
}

void ws::Kernel::delete_process_event(int ident) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_PROC, EV_DELETE, 0, 0, NULL);
  _change_list.push_back(event);
}

void ws::Kernel::delete_user_event(int ident) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_USER, EV_DELETE, 0, 0, NULL);
  _change_list.push_back(event);
}
