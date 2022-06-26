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
#include "RequestMessage.hpp"

namespace ws {

  class Socket {

  public:
    typedef std::map<int, struct sockaddr_in> server_type;
    typedef std::map<int, ws::RequestMessage*> client_type;
    typedef struct kevent*                    kevent_pointer;
    typedef void (*kevent_func)(ws::Socket::kevent_data* info);

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

    struct kevent_data {
      ws::Socket*     self;
      struct kevent*  event;
      ws::RequestMessage*     request;
      // ws::ResponseMessage*  response;
      kevent_func func;
    };

    /* ====================================== */
    /*                  OCCF                  */
    /* ====================================== */

    Socket& operator=(const Socket& cls);
    Socket(const Socket& cls);
    Socket();

    /* ====================================== */
    /*            Private Function            */
    /* ====================================== */

    static void connect_client(kevent_data* info);
    static void parse_request(kevent_data* info);
    static void send_response(kevent_data* info);

    kevent_data init_kevent_udata(void* func, ws::RequestMessage* request);

  public:

    /* ====================================== */
    /*                Structor                */
    /* ====================================== */

    Socket(const ws::Configure& cls);
    ~Socket();

    /* ====================================== */
    /*            Public Function             */
    /* ====================================== */

    void request_handler();
  };
}