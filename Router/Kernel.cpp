#include "Kernel.hpp"

ws::Kernel::Kernel() {
  _kq = kqueue();
  if (_kq == -1)
    throw; // require custom exception
}

ws::Kernel::~Kernel() {}

/*
  커널에서 이벤트 핸들링 하고 싶은 kevent 구조체 등록
*/
void  ws::Kernel::kevent_ctl(uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
  struct kevent event;

  EV_SET(&event, ident, filter, flags, fflags, data, udata);
  if (kevent(_kq, &event, 1, NULL, 0, NULL) == -1)
    throw; // require custom exception
}

/*
  커널에서 발생한 이벤트 리턴해주는 함수
*/
#include <stdlib.h>
int ws::Kernel::kevent_wait(struct kevent* event_list) {
  int event_size;

  memset(event_list, 0, sizeof(event_list));
  if ((event_size = kevent(_kq, NULL, 0, event_list, 80, NULL)) == -1)
    throw; // require custom exception
  return event_size;
}
