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
#include "Kernel.hpp"
#include "Request.hpp"

namespace ws {

  class Socket {

  private:
    struct kevent_data {
      ws::Socket*     self;
      struct kevent*  event;
      ws::Request*     request;
      // ws::ResponseMessage*  response;
      void (*func)(struct kevent_data* info);
    };

  public:
    typedef struct kevent_data    kevent_data;
    typedef std::map<int, ws::Configure::listen_type> server_type;
    typedef std::map<int, ws::Request> client_type;
    typedef void (*kevent_func)(ws::Socket* self, struct kevent event);

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

    ws::Kernel  _kernel;

    /* ====================================== */
    /*                  OCCF                  */
    /* ====================================== */

    Socket& operator=(const Socket& cls);
    Socket(const Socket& cls);
    Socket();

    /* ====================================== */
    /*            Private Function            */
    /* ====================================== */

    static void connect_client(ws::Socket* self, struct kevent event);
    static void parse_request(ws::Socket* self, struct kevent event);
    static void send_response(ws::Socket* self, struct kevent event);

    kevent_data init_kevent_udata(kevent_func func, ws::Request* request);

  public:

    /* ====================================== */
    /*                Structor                */
    /* ====================================== */

    Socket(int port); // 지워야함
    Socket(const ws::Configure& cls);
    ~Socket();

    /* ====================================== */
    /*            Public Function             */
    /* ====================================== */

    void request_handler();
  };
}
