#pragma once

#include <cstdint>

#include <sys/event.h> // kevent header
#include <sys/time.h> // kevent header
#include <sys/types.h> // kevent header
#include <unistd.h>

namespace ws {
  class Kernel {
  
  private:
    int _kq;

    Kernel(const Kernel& cls);
    Kernel& operator=(const Kernel& cls);

    /*
      On call pending state until kernel fetches new events
    */
    void  kevent_ctl(
      uintptr_t ident,
      int16_t filter,
      uint16_t flags,
      uint32_t fflags,
      intptr_t data,
      void *udata
    );

  public:
    Kernel() throw();
    ~Kernel();

    int   kevent_wait(struct kevent* event_list, size_t event_size);

    void  add_signal_event(int ident, void* udata, uint16_t flags = 0, uint32_t fflags = 0, intptr_t data = 0);

    void  add_read_event(int ident, void* udata, uint16_t flags = 0, uint32_t fflags = 0, intptr_t data = 0);
    void  add_write_event(int ident, void* udata, uint16_t flags = 0, uint32_t fflags = 0, intptr_t data = 0);
    void  add_process_event(int ident, void* udata, uint16_t flags = 0, uint32_t fflags = 0, intptr_t data = 0);
    void  add_user_event(int ident, void* udata, uint16_t flags = 0, uint32_t fflags = 0, intptr_t data = 0);

    void  delete_read_event(int ident);
    void  delete_write_event(int ident);
    void  delete_process_event(int ident);
    void  delete_user_event(int ident);
  };
}
