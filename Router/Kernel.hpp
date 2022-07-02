#pragma once

#include <sstream>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <map>

namespace ws {
  class Kernel {
  
  private:
    int _kq;

    Kernel(const Kernel& cls);
    Kernel& operator=(const Kernel& cls);

  public:
    Kernel();
    ~Kernel();

    /*
      On call pending state until kernel fetches new events
    */
    void            kevent_ctl(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
    struct kevent*  kevent_wait();
  };
}
