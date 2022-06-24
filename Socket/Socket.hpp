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

#include "Configure.hpp"

namespace ws {

  class Socket {
  
  public:
    typedef std::map<int, struct sockaddr_in> server_type;
    typedef server_type::iterator (Socket::*get_server_info_type) (int);
    typedef std::map<int, get_server_info_type>   client_type;
    typedef std::vector<struct kevent>        kevent_vector;

  private:

    /* ====================================== */
    /*             Member Variable            */
    /* ====================================== */

    /*
      first: server socket fd
      second: server socket address info
    */
    server_type _server;

    /*
      first: client socket fd
      second: server socket fd
    */
    client_type _client;

    /*
      새로 할당할 이벤트 목록을 갖고있는 벡터
    */
    kevent_vector _change_list;

    /*
      핸들링이 필요한 이벤트 목록을 담는 벡터
    */
    kevent_vector _event_list;

    /*
      kernel descriptor
    */
    int _kq;

    /* ====================================== */
    /*                  OCCF                  */
    /* ====================================== */

    Socket& operator=(const Socket& cls);
    Socket(const Socket& cls);
    Socket();

    /* ====================================== */
    /*            Private Function            */
    /* ====================================== */

    /*
      insert new kevent to vector
    */
    void add_new_kevent(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata);

    server_type::iterator get_server_info(int fd);

  public:

    /* ====================================== */
    /*                Structor                */
    /* ====================================== */

    Socket(const Configure& cls);
    ~Socket();

    /* ====================================== */
    /*            Public Function             */
    /* ====================================== */

    void kernel_handler();
  };
}