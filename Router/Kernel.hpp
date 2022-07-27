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
    const static std::size_t kDefaultEventListSize;

    std::vector<struct kevent> _change_list;
    std::vector<struct kevent> _event_list;
    std::vector<struct kevent> _delete_list;

    Kernel(const Kernel& cls);
    Kernel& operator=(const Kernel& cls);

  public:
    Kernel() throw();
    ~Kernel();

    const struct kevent* get_event_list() const throw();

    int   kevent_ctl(int event_size);

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
