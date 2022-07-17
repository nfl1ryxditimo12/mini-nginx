#pragma once

#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>

#include "Configure.hpp"
#include "Kernel.hpp"
#include "Request.hpp"
#include "Repository.hpp"

namespace ws {
  class Validator;
  class Response;

  class Socket {

  private:
    struct client_data {

      client_data(ws::Configure::listen_type listen)
      : fatal(false), status(0), repository(ws::Repository(fatal, status)), request(ws::Request(listen)), response(""), write_offset(0) {};

      client_data(const client_data& cls)
      : fatal(cls.fatal), status(cls.status), repository(cls.repository), request(cls.request), response(cls.response), write_offset(cls.write_offset) {};

      bool                    fatal;
      unsigned int            status;
      ws::Repository          repository;
      ws::Request             request;
      std::string             response;
      std::string::size_type  write_offset;
    };

  public:
    typedef ws::Configure::listen_type                listen_type;

    typedef std::pair<int, ws::Configure::listen_type> server_type;
    typedef std::map<server_type::first_type, server_type::second_type> server_map_type;

    typedef struct client_data                                          client_value_type;
    typedef std::pair<unsigned int, client_value_type>                           client_type;
    typedef std::map<client_type::first_type, client_type::second_type> client_map_type;

    typedef void (*kevent_func)(struct kevent event);

  private:

    /* ====================================== */
    /*             Member Variable            */
    /* ====================================== */

    static ws::Configure _conf;

    static ws::Kernel _kernel;

    /*
      first: server socket fd
      second: server socket address info
    */
    static server_map_type _server;

    /*
      first: client socket fd
      second: client data pointer
    */
    static client_map_type _client;

    static ws::Validator _validator;
    static ws::Response  _response;
    const static std::size_t kBUFFER_SIZE;

    /* ====================================== */
    /*                  OCCF                  */
    /* ====================================== */

    Socket& operator=(const Socket& cls);
    Socket(const Socket& cls);
    Socket();

    /* ====================================== */
    /*            Private Function            */
    /* ====================================== */

    static void init_client(unsigned int fd, listen_type listen);
    static void disconnect_client(int fd);
    static void exit_socket();

    /* control socket to kenel event */
    static void connect_client(struct kevent event);
    static void recv_request(struct kevent event);
    static void process_request(struct kevent event);
    static ws::Socket::client_map_type::iterator find_client_by_file(int file) throw();

  public:

    /* ====================================== */
    /*                Structor                */
    /* ====================================== */

    ~Socket();

    /* ====================================== */
    /*            Public Function             */
    /* ====================================== */

    static void init_server(const ws::Configure& conf);
    static void run_server();

    static void send_response(struct kevent event);
    /* control data to kenel event */
    static void read_data(struct kevent event);
    static void write_data(struct kevent event);

    static void generate_response(struct kevent event);
  };
}
