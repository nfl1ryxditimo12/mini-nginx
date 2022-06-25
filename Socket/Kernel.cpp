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

const ws::Kernel::kevent_vector& ws::Kernel::event_handler() {
  _event_list.clear();
  if (kevent(_kq, &_change_list[0], _change_list.size(), &_event_list[0], _event_list.capacity(), NULL) == -1)
    throw; // require custom exception
  _change_list.clear();
  return _event_list;
}

void ws::Kernel::add_change_list(uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata) throw()
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    _change_list.push_back(temp_event);
}