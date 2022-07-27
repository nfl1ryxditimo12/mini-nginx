#pragma once

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>

#include "Configure.hpp"
#include "Request.hpp"
#include "Define.hpp"
#include "Util.hpp"

namespace ws {

  #define FD_DEFAULT -2

  class Repository {

  public:
    typedef ws::Server::listen_type listen_type;
    typedef ws::Server::server_name_type server_name_type;
    typedef std::string location_dir_type;
    typedef ws::Server::location_map_type location_map_type;
    typedef ws::Location::session_type session_type;
    typedef ws::Location::limit_except_vec_type limit_except_vec_type;
    typedef ws::Location::return_type redirect_type;
    typedef ws::InnerOption::autoindex_type autoindex_bool_type;
    typedef ws::InnerOption::root_type root_type;
    typedef ws::InnerOption::index_set_type index_set_type;
    typedef ws::InnerOption::index_check_type index_check_type;
    typedef ws::InnerOption::client_max_body_size_type client_max_body_size_type;
    typedef ws::InnerOption::error_page_map_type error_page_map_type;

  private:
    struct config_data {
      /*server*/
      listen_type listen;
      server_name_type server_name;
    /*location*/
      limit_except_vec_type limit_except_vec;
      redirect_type redirect;
    /*option*/
      autoindex_bool_type autoindex;
      root_type root;
      index_set_type index;
      index_check_type index_check;
      client_max_body_size_type client_max_body_size;
      error_page_map_type error_page_map;

      config_data() {}

      config_data(const config_data& other)
        : listen(other.listen), server_name(other.server_name), limit_except_vec(other.limit_except_vec),
          redirect(other.redirect), autoindex(other.autoindex),
          root(other.root), index(other.index), index_check(other.index_check), client_max_body_size(other.client_max_body_size),
          error_page_map(other.error_page_map) {}

      config_data& operator=(const config_data& other) {
        listen = other.listen;
        server_name = other.server_name;
        limit_except_vec = other.limit_except_vec;
        redirect = other.redirect;
        autoindex = other.autoindex;
        root = other.root;
        index = other.index;
        client_max_body_size = other.client_max_body_size;
        error_page_map = other.error_page_map;
        return *this;
      }
    };

  public:

    typedef struct ws::Repository::config_data      config_type;

    typedef std::vector<std::string>                autoindex_type;
  
  private:

    /* =================================== */
    /*      Repository member variable     */
    /* =================================== */

    bool _fatal;

    unsigned int _status; // get_status();

    bool _session;

    int _fd; // get_fd();

    struct stat _file_stat;
    bool        _file_exist_stat;

    std::string _index_root;

    std::string _uri;

    std::string _file_path;

    std::string _host; // get_host();

    std::string _method; // get_method();

    std::string _request_body;

    autoindex_type _autoindex; // get_autoindex();

    std::string _content_type; // get_conent_type();

    const ws::Server*   _server;
    const ws::Location* _location;
    const ws::Request*  _request;

    /* =================================== */
    /*         Non-getter variable         */
    /* =================================== */

    config_type _config;

    /* =================================== */
    /*                 OCCF                */
    /* =================================== */

    Repository();
    Repository& operator=(const Repository& cls);

    void set_option(const ws::InnerOption& option);
    void set_autoindex();
    void set_content_type();

    void open_file(std::string filename);
    void open_error_html();

  public:
    Repository(bool fatal, unsigned int status);
    Repository(const Repository& cls);
    ~Repository();

    void operator()(const ws::Server& server, const ws::Request& request);

    void set_repository(unsigned int status);
    void set_status(unsigned int status);
    void set_fatal(); // always set true

    /* =================================== */
    /*                Getter               */
    /* =================================== */

    const ws::Server* get_server() const throw();
    const ws::Location* get_location() const throw();

    bool                  is_fatal() const throw();
    const int&            get_fd() const throw();
    const unsigned int&   get_status() const throw();
    bool                  is_session() const throw();
    const struct stat&    get_file_stat() const throw();
    bool                  get_file_exist_stat() const throw();
    const std::string&    get_content_type() const throw();
    const std::string&    get_method() const throw();
    const std::string&    get_file_path() const throw();
    const autoindex_type& get_autoindex() const throw();
    const redirect_type&  get_redirect() const throw();
    const client_max_body_size_type& get_client_max_body_size() const throw();

    // clear for keep-alive
    void clear() throw();
  };
}
