#pragma once

#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>
#include <sys/time.h>
#include <cmath>

#include "Configure.hpp"
#include "CgiHandler.hpp"
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
        : fatal(false), status(0), repository(ws::Repository(fatal, status)), request(ws::Request(listen)),
          response_header(), response_body(), write_offset(0), pipe_offset(), cgi_handler(), is_cgi_header(true), start_time(clock()) {
        gettimeofday(&connect_time, NULL);
      };

//      client_data(const client_data& cls) // todo: ???
//      : fatal(cls.fatal), status(cls.status), repository(cls.repository), request(cls.request), response(cls.response), write_offset(cls.write_offset), start_time(cls.start_time), session_iter(cls.session_iter) {};

      bool                    fatal;
      unsigned int            status;
      ws::Repository          repository;
      ws::Request             request;
      std::string             response_total;
      std::string             response_header;
      std::string             response_body;
      std::string::size_type  write_offset;
      std::string::size_type  pipe_offset;
      ws::CgiHandler          cgi_handler;
      pid_t                   cgi_pid;
      bool                    is_cgi_header;
      ws::Buffer              buffer;
      clock_t                 start_time; // todo: function start time
      struct timeval          connect_time; // todo: socket connect time
    };

    struct session_data {
        session_data(const std::string& name): hit_count(0), name(name) {};

        unsigned int hit_count;
        std::string name;
    };

  public:
    typedef ws::Configure::listen_type                                    listen_type;

    typedef std::pair<int, ws::Configure::listen_type>                    server_type;
    typedef std::map<server_type::first_type, server_type::second_type>   server_map_type;

    typedef struct client_data                                            client_value_type;
    typedef std::pair<unsigned int, client_value_type>                    client_type;
    typedef std::map<client_type::first_type, client_type::second_type>   client_map_type;

    typedef struct session_data                                           session_value_type;
    typedef std::pair<unsigned int, session_value_type>                   session_type;
    typedef std::map<session_type::first_type, session_type::second_type> session_map_type;

    typedef void (*kevent_func)(const struct kevent& event, client_map_type::iterator& client);

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

    static session_map_type _session;
    static unsigned int _session_index;

    static sig_atomic_t _signal;

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
    static void accecpt_signal(const struct kevent& event, client_map_type::iterator& client);
    static void connect_client(const struct kevent& event, client_map_type::iterator& client);
    static void recv_request(const struct kevent& event, client_map_type::iterator& client);
    static void process_request(const struct kevent& event, client_map_type::iterator& client);
    static void parse_cgi_return(client_value_type& client);
    static ws::Socket::client_map_type::iterator find_client_by_file(int file) throw();
    static ws::Socket::client_map_type::iterator find_client_by_fpipe(int fpipe) throw();
    static ws::Socket::client_map_type::iterator find_client_by_bpipe(int bpipe) throw();
    static ws::Socket::client_map_type::iterator find_client_by_pid(pid_t pid) throw();

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

    static void send_response(const struct kevent& event, client_map_type::iterator& client);
    /* control data to kenel event */
    static void process_session(const struct kevent& event, client_map_type::iterator& client);
    static void read_data(const struct kevent& event, client_map_type::iterator& client);
    static void write_data(const struct kevent& event, client_map_type::iterator& client);
    static void read_pipe(const struct kevent& event, client_map_type::iterator& client);
    static void write_pipe(const struct kevent& event, client_map_type::iterator& client);
    static void wait_child(const struct kevent& event, client_map_type::iterator& client);

    static void generate_response(int client_fd, client_value_type& client_data);
  };
}
