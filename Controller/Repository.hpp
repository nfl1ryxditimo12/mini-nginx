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

/*
  bool autoindex;
  vector<string> dir; // file or dir name vector
  string filename; // 

  content-type;

  그외 필요한 데이터 만들어야함

*/


namespace ws {

  #define FD_DEFAULT -2

  class Repository {

  public:
    typedef ws::Server::listen_type listen_type;
    typedef ws::Server::server_name_type server_name_type;
    typedef std::string location_dir_type;
    typedef ws::Server::location_map_type location_map_type;
    typedef ws::Location::limit_except_vec_type limit_except_vec_type;
    typedef ws::Location::return_type redirect_type;
    typedef ws::InnerOption::autoindex_type autoindex_bool_type;
    typedef ws::InnerOption::root_type root_type;
    typedef ws::InnerOption::index_set_type index_set_type;
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
      client_max_body_size_type client_max_body_size;
      error_page_map_type error_page_map;

      config_data() {}

      config_data(const config_data& other)
        : listen(other.listen), server_name(other.server_name), limit_except_vec(other.limit_except_vec),
          redirect(other.redirect), autoindex(other.autoindex),
          root(other.root), index(other.index), client_max_body_size(other.client_max_body_size),
          error_page_map(other.error_page_map) {}

      config_data& operator=(const config_data& other) {
        config_data temp(other);
        std::swap(*this, temp);
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

    /*
      default root dir 있으면 좋을듯 char* 또는 std::string
      return 키워드 처리 어떻게 해야할지 고민해봐야함
      _root 변수 char*로 처리하는게 어떨까 (.c_str() 함수 남발하기 싫음)

    */

    /* 치명적인 오류 일 경우 콘솔 또는 throw 하는 방식 생각해 봐야함 */

    bool _fatal;

    unsigned int _status; // get_status();

    int _fd; // get_fd();

    struct stat _file_stat;

    std::string _index_root;

    /* filename 필요함 절대경로로 */
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
    void set_fd(int value);

    void test();

    /* =================================== */
    /*                Getter               */
    /* =================================== */

    const ws::Server* get_server() const throw();
    const ws::Location* get_location() const throw();

    bool                  is_fatal() const throw();
    const int&            get_fd() const throw();
    const unsigned int&   get_status() const throw();
    const struct stat&    get_file_stat() const throw();
    const std::string&    get_host() const throw();
    const std::string&    get_method() const throw();
    const std::string&    get_root() const throw();
    const std::string&    get_index_root() const throw();
    const std::string&    get_uri() const throw();
    const std::string&    get_file_path() const throw();
    const std::string&    get_request_body() const throw();
    const autoindex_type& get_autoindex() const throw();
    const std::string&    get_content_type() const throw();
    const redirect_type&  get_redirect() const throw();

    // clear for keep-alive
    void clear() throw();
  };
}
