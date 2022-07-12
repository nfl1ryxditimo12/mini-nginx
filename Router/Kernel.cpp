#include "Kernel.hpp"

ws::Kernel::Kernel() {
  _kq = kqueue();
  if (_kq == -1)
    throw; // require custom exception
}

ws::Kernel::~Kernel() {
  close(_kq);
}

/*
  커널에서 이벤트 핸들링 하고 싶은 kevent 구조체 등록
*/

void  ws::Kernel::kevent_ctl(uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata, struct timespec* limit) {
  struct kevent event;
  EV_SET(&event, ident, filter, flags, fflags, data, udata);
  if (kevent(_kq, &event, 1, NULL, 0, limit) == -1)
    throw; // require custom exception
}

/*
  커널에서 발생한 이벤트 리턴해주는 함수
*/
#include <stdlib.h>
int ws::Kernel::kevent_wait(struct kevent* event_list, size_t event_size) {
  int new_event;

  memset(event_list, 0, sizeof(struct kevent) * event_size);
  if ((new_event = kevent(_kq, NULL, 0, event_list, event_size, NULL)) == -1)
    throw; // require custom exception
  return new_event;
}
