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
#include "Repository.hpp"
// #include "Validator.hpp"

namespace ws {

  class Response;

  class Socket {

  private:
    struct client_data {
      ws::Request*      request;
      ws::Repository*   repository;
      ws::Response*     response;
      unsigned int      status;
    };

  public:
    typedef ws::Configure::listen_type                listen_type;

    typedef std::pair<int, ws::Configure::listen_type> server_type;
    typedef std::map<server_type::first_type, server_type::second_type> server_map_type;

    typedef struct client_data                                          client_value_type;
    typedef std::pair<int, client_value_type*>                          client_type;
    typedef std::map<client_type::first_type, client_type::second_type> client_map_type;

    typedef void (*kevent_func)(ws::Socket* self, struct kevent event);

  private:

    /* ====================================== */
    /*             Member Variable            */
    /* ====================================== */

    const ws::Configure* _conf;

    ws::Kernel _kernel;


    /*
      first: server socket fd
      second: server socket address info
    */
    server_map_type _server;

    /*
      first: client socket fd
      second: client data pointer
    */
    client_map_type _client;

    /* ====================================== */
    /*                  OCCF                  */
    /* ====================================== */

    Socket& operator=(const Socket& cls);
    Socket(const Socket& cls);
    Socket();

    /* ====================================== */
    /*            Private Function            */
    /* ====================================== */

    void init_client(int fd, listen_type listen, client_value_type* client_data);
    void disconnect_client(int fd);
    void exit_socket();

    static void connect_client(ws::Socket* self, struct kevent event);
    static void recv_request(ws::Socket* self, struct kevent event);
    static void process_request(ws::Socket* self, struct kevent event);
    static void send_response(ws::Socket* self, struct kevent event);

  public:

    /* ====================================== */
    /*                Structor                */
    /* ====================================== */

    Socket(const ws::Configure& cls);
    ~Socket();

    /* ====================================== */
    /*            Public Function             */
    /* ====================================== */

    void connection();
  };
}
