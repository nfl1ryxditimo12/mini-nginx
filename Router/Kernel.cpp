#include "Kernel.hpp"

#include <cstdlib>
#include <cstring>

//extern bool webserv_fatal;

ws::Kernel::Kernel() throw() {
  try {
    _kq = kqueue();
    if (_kq == -1)
      throw; // require custom exception
  } catch (...) {
//    webserv_fatal = true;
  }
}

ws::Kernel::~Kernel() {
  close(_kq);
}

/*
  커널에서 이벤트 핸들링 하고 싶은 kevent 구조체 등록
*/
#include <stdio.h>
void  ws::Kernel::kevent_ctl(uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
  struct kevent event;
  EV_SET(&event, ident, filter, flags, fflags, data, udata);
  if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1) {
    perror("kevent");
    throw; // require custom exception
  }
}
/*
  커널에서 발생한 이벤트 리턴해주는 함수
*/
int ws::Kernel::kevent_wait(struct kevent* event_list, size_t event_size) {
  int new_event;

  std::memset(event_list, 0, sizeof(struct kevent) * event_size);
  if ((new_event = kevent(_kq, NULL, 0, event_list, event_size, NULL)) == -1)
    throw; // require custom exception
  return new_event;
}

void ws::Kernel::add_read_event(int fd, void *udata, uint16_t flags, uint32_t fflags, intptr_t data) {
  kevent_ctl(fd, EVFILT_READ, EV_ADD | flags, fflags, data, udata);
}

void ws::Kernel::add_write_event(int fd, void *udata, uint16_t flags, uint32_t fflags, intptr_t data) {
  kevent_ctl(fd, EVFILT_WRITE, EV_ADD | flags, fflags, data, udata);
}

void ws::Kernel::add_process_event(int fd, void *udata, uint16_t flags, uint32_t fflags, intptr_t data) {
  kevent_ctl(fd, EVFILT_USER, EV_ADD | flags, NOTE_TRIGGER | fflags, data, udata);
}

void ws::Kernel::delete_read_event(int fd) {
  kevent_ctl(fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
}

void ws::Kernel::delete_write_event(int fd) {
  kevent_ctl(fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
}

void ws::Kernel::delete_process_event(int fd) {
  kevent_ctl(fd, EVFILT_USER, EV_DELETE, 0, 0, NULL);
}
