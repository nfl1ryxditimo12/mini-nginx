#include "Kernel.hpp"

#include <cstdio>
#include <errno.h>

extern bool webserv_fatal;

const std::size_t ws::Kernel::kDefaultEventListSize = 1024;

ws::Kernel::Kernel() throw() : _change_list(), _event_list(), _delete_list() {
  try {
    _kq = kqueue();

    if (_kq == -1)
      throw std::runtime_error("kq error");

    _event_list.reserve(kDefaultEventListSize);
  } catch (...) {
    webserv_fatal = true;
  }
}

ws::Kernel::~Kernel() {
  close(_kq);
  _change_list.clear();
}

const struct kevent* ws::Kernel::get_event_list() const throw() {
  return &_event_list[0];
}

int ws::Kernel::kevent_ctl(int event_size) {
  int new_event;

  if (_event_list.capacity() < static_cast<std::vector<struct kevent>::size_type>(event_size))
    _event_list.reserve(event_size);

  if ((kevent(_kq, _delete_list.data(), _delete_list.size(), NULL, 0, NULL) == -1) && (errno != ENOENT)) {
    std::perror("delete");
    throw std::runtime_error("kevent delete error");
  }

  _delete_list.clear();

  new_event = kevent(_kq, _change_list.data(), _change_list.size(), &_event_list[0], event_size, NULL);
  if (new_event == -1)
    throw std::runtime_error("kevent ctl error");

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
  _delete_list.push_back(event);
}

void ws::Kernel::delete_write_event(int ident) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  _delete_list.push_back(event);
}

void ws::Kernel::delete_process_event(int ident) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_PROC, EV_DELETE, 0, 0, NULL);
  _delete_list.push_back(event);
}

void ws::Kernel::delete_user_event(int ident) {
  struct kevent event;
  EV_SET(&event, ident, EVFILT_USER, EV_DELETE, 0, 0, NULL);
  _delete_list.push_back(event);
}
