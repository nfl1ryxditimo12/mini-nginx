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
    typedef ws::Location::return_type return_type;
    typedef ws::Location::cgi_type cgi_type;
    typedef ws::InnerOption::autoindex_type autoindex_type;
    typedef ws::InnerOption::root_type root_type;
    typedef ws::InnerOption::index_vec_type index_vec_type;
    typedef ws::InnerOption::client_max_body_size_type client_max_body_size_type;
    typedef ws::InnerOption::error_page_map_type error_page_map_type;

  private:
    struct config_data {
      /*server*/
      const listen_type* listen;
      const server_name_type* server_name;
    /*location*/
      const limit_except_vec_type* limit_except_vec;
      const return_type* redirect;
      const cgi_type* cgi;
    /*option*/
      const autoindex_type* autoindex;
      const root_type* root;
      const index_vec_type* index;
      const client_max_body_size_type* client_max_body_size;
      const error_page_map_type* error_page_map;
    };

  public:

    typedef struct ws::Repository::config_data      config_type;
  
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

    int _fd; // get_fd();

    int _status; // get_status();

    std::string _host; // get_host();

    std::string _method; // get_method();

    std::string _request_body;

    std::vector<std::string> _autoindex; // get_autoindex();

    std::pair<std::string, char**> _cgi; // get_cgi();

    std::string _content_type; // get_conent_type();

    const ws::Server*   _server;
    const ws::Location* _location;
    const ws::Request*  _request;

    /* =================================== */
    /*         Non-getter variable         */
    /* =================================== */

    config_type _config;
    // bool _dir;
    // struct stat _file;
    std::string _root;
    return_type _return; // 어떻게 처리해야 하는지 내일 상의해야함

    /* =================================== */
    /*                 OCCF                */
    /* =================================== */

    Repository(const Repository& cls);
    Repository& operator=(const Repository& cls);

    void set_option(const ws::InnerOption& option);
    void set_status(const int& status);
    void set_autoindex();
    void set_content_type();

    void open_file(std::string filename);

  public:
    Repository();
    ~Repository();

    void operator()(const ws::Server* server, const ws::Request* request);

    void set_repository(int status);

    /* =================================== */
    /*                Getter               */
    /* =================================== */

    const ws::Server* get_server() const throw();
    const ws::Location* get_location() const throw();
  };
}
