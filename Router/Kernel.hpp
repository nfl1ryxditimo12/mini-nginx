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
  
  public:
    typedef std::vector<struct kevent>        kevent_vector;
  
  private:
  
    int _kq;

    /*
      새로 할당할 이벤트 목록을 갖고있는 벡터
    */
    kevent_vector _change_list;

    /*
      핸들링이 필요한 이벤트 목록을 담는 벡터
    */
    kevent_vector _event_list;

    Kernel(const Kernel& cls);
    Kernel& operator=(const Kernel& cls);

  public:
    Kernel();
    ~Kernel();

    void  resize_event_list(int server_count);
    void  reset_event_list();
    void  reset_change_list();

    /*
      On call pending state until kernel fetches new events
    */
    void            kevent_ctl(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
    struct kevent   kevent_wait();


    /*
      Append data to look up to the kevent struct
    */
    struct kevent add_change_list(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) throw();
  };
}
