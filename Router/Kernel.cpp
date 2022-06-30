#include "Kernel.hpp"

ws::Kernel::Kernel() {
  _kq = kqueue();
  if (_kq == -1)
    throw; // require custom exception
}

ws::Kernel::~Kernel() {}

void  ws::Kernel::resize_event_list(int server_count) {
  _event_list.resize(server_count * 8);
}

void  ws::Kernel::reset_event_list() {
  for (size_t i = 0; i < _event_list.size(); i++)
    memset(&_event_list[i], 0, sizeof(_event_list[i]));
}

void  ws::Kernel::reset_change_list() {
  for (size_t i = 0; i < _change_list.size(); i++)
    memset(&_change_list[i], 0, sizeof(_change_list[i]));
  _change_list.clear();
}

void  ws::Kernel::kevent_ctl(uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
  struct kevent event;

  EV_SET(&event, ident, filter, flags, fflags, data, udata);
  if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
    throw; // require custom exception
}

struct kevent ws::Kernel::kevent_wait() {
  // reset_event_list();
  struct kevent event_list;
  if (kevent(_kq, NULL, 0, &event_list, 1, NULL) == -1)
    throw; // require custom exception
  return event_list;
}

struct kevent ws::Kernel::add_change_list(uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata) throw()
{
    struct kevent event;

    EV_SET(&event, ident, filter, flags, fflags, data, udata);
    return event;
}
