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
  _timeout.tv_sec = 2;
  _timeout.tv_nsec = 0;
}

ws::Kernel::~Kernel() {
  close(_kq);
  _change_list.clear();
}

/*
  커널에서 이벤트 핸들링 하고 싶은 kevent 구조체 등록
*/

void  ws::Kernel::kevent_ctl(uintptr_t ident, int16_t filter,
                             uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
  struct kevent event;
  EV_SET(&event, ident, filter, flags, fflags, data, udata);
  if (kevent(_kq, &event, 1, NULL, 0, &_timeout) == -1) {
    perror("kevent");
    throw; // require custom exception
  }
}
/*
  커널에서 발생한 이벤트 리턴해주는 함수
*/

// todo: 네이밍 변경
int ws::Kernel::kevent_wait(struct kevent* event_list, size_t event_size) {
  int new_event;

  std::memset(event_list, 0, sizeof(struct kevent) * event_size);
  if ((new_event = kevent(_kq, &_change_list[0], _change_list.size(), event_list, event_size, &_timeout)) == -1)
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

void ws::Kernel::delete_event(int fd, int16_t filter) {
  kevent_ctl(fd, filter, EV_DELETE, 0, 0, NULL);
}
