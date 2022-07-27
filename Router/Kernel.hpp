#pragma once

#include <cstdint>

#include <sys/event.h> // kevent header
#include <sys/time.h> // kevent header
#include <sys/types.h> // kevent header
#include <unistd.h>
#include <vector>

namespace ws {
  class Kernel {
  
  private:
    int _kq;

    std::vector<struct kevent> _change_list;
    struct timespec _timeout;

    Kernel(const Kernel& cls);
    Kernel& operator=(const Kernel& cls);

    /*
      On call pending state until kernel fetches new events
    */

  public:
    Kernel() throw();
    ~Kernel();

    void  kevent_ctl(
            uintptr_t ident,
            int16_t filter,
            uint16_t flags,
            uint32_t fflags,
            intptr_t data,
            void *udata
    );
    int   kevent_wait(struct kevent* event_list, size_t event_size);

    void  add_signal_event(int ident, void* udata, uint16_t flags = 0, uint32_t fflags = 0, intptr_t data = 0);

    void  add_read_event(int ident, void* udata, uint16_t flags = 0, uint32_t fflags = 0, intptr_t data = 0);
    void  add_write_event(int ident, void* udata, uint16_t flags = 0, uint32_t fflags = 0, intptr_t data = 0);
    void  add_process_event(int ident, void* udata, uint16_t flags = 0, uint32_t fflags = 0, intptr_t data = 0);
    void  add_user_event(int ident, void* udata, uint16_t flags = 0, uint32_t fflags = 0, intptr_t data = 0);

    void  delete_event(int ident, int16_t filter);
  };
}
