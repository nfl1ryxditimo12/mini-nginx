#include "Repository.hpp"
#include "Request.hpp"


#include <iostream> // todo
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"
/*
  todo

  fatal 인 경우 전체적인 흐름 유지하는 처리

  테스트 하며 멤버 콘솔 출력 // seonkim
  _root + _uri 중간에 슬래시 붙는지 확인해야함
  파일 오픈 분기 delete 처리 해줘야함
  권한 없을 때 파일 열기 어떻게 처리할지
*/

ws::Repository::Repository(bool fatal, unsigned int status): _fatal(fatal), _status(status), _fd(FD_DEFAULT) {
  memset(&_file_stat, 0, sizeof(struct stat));
  _index_root = ws::Util::get_root_dir() + "/www";
}

ws::Repository::Repository(const Repository& cls): _fatal(cls._fatal), _status(cls._status) {
  _fd = cls._fd;
  _uri = cls._uri;
  _file_path = cls._file_path;
  _host = cls._host;
  _method = cls._method;
  _request_body = cls._request_body;
  _autoindex = cls._autoindex;
  _cgi = cls._cgi;
  _content_type = cls._content_type;
  _server = cls._server;
  _location = cls._location;
  _request = cls._request;

  _config = cls._config;
  _index_root = cls._index_root;
}

ws::Repository::~Repository() {}

void ws::Repository::operator()(const ws::Server& server, const ws::Request& request) {
  _request = &request;
  _request_body = _request->get_request_body();
  _server = &server;
  _location = &(_server->find_location(Util::parse_relative_path(_request->get_uri())));

  _uri = _request->get_uri().substr(_location->get_block_name().length());;

  /*set server*/
  _config.listen = request.get_listen();
  _config.server_name = request.get_server_name();

  if (_location != NULL) {
    _config.limit_except_vec = _location->get_limit_except_vec();
    _config.redirect = _location->get_return();
    _config.cgi = _location->get_cgi();
/*set option*/
    ws::Repository::set_option(_location->get_option());
  } else
    ws::Repository::set_option(_server->get_option());

  _file_path = _config.root + (_uri[0] == '/' || !_uri.length() ? "" : "/") + _uri;

  lstat(_file_path.c_str(), &_file_stat);

    std::string server_name = _request->get_server_name() == "_" ? "localhost" : _request->get_server_name();

  _host = server_name + ":" + ws::Util::ultos(ntohs(_config.listen.second));
  _method = _request->get_method();

  // todo: test print
  std::cout << YLW << "\n=========================================================\n" << NC << std::endl;
  std::cout << RED << _method << " " << _location->get_block_name() << NC << std::endl;
}

void ws::Repository::set_option(const ws::InnerOption& option) {
  _config.autoindex = option.get_autoindex();
  _config.root = option.get_root();
  _config.index = option.get_index_set();
  _config.client_max_body_size = option.get_client_max_body_size();
  _config.error_page_map = option.get_error_page_map();
}

void ws::Repository::set_repository(unsigned int value)  {
  this->set_status(value);
  if (_config.redirect.first > 0)
    this->set_status(_config.redirect.first); // todo

  if (_status == 0) {
    if (S_ISDIR(_file_stat.st_mode))
      this->set_autoindex();
    else
      this->open_file(_file_path);
  }
  
  if (_status >= BAD_REQUEST)
    this->open_error_html();

  if (_status == 0)
    this->set_status(_method == "POST" || _method == "PUT" ? 200 : 200);

  this->set_content_type();

//  test(); // todo: print test
}

void ws::Repository::set_status(unsigned int status) {
  if (_status >= BAD_REQUEST)
    return;
  _status = status;
}

void ws::Repository::set_fatal() {
  _fatal = true;
}

void ws::Repository::set_fd(int value) {
  _fd = value;
}

void ws::Repository::set_autoindex() {

  if (!(_method == "GET" || _method == "HEAD")) {
    this->set_status(METHOD_NOT_ALLOWED);
    return;
  }

  DIR* dir = opendir(_file_path.c_str());
  struct dirent *file    = NULL;

  if (dir == NULL)
    this->set_status(INTERNAL_SERVER_ERROR);

  while (dir && ((file = readdir(dir)) != NULL)) {
    index_set_type::const_iterator filename = _config.index.find(file->d_name);

    if (filename != _config.index.end()) {
      open_file(_file_path + "/" + *filename);
      _autoindex.clear();
      break;
    }

    if (_config.autoindex)
      _autoindex.push_back(file->d_name);
  }

  if (_fd == FD_DEFAULT && !_config.autoindex && !(_method == "HEAD" || _method == "DELETE"))
    this->set_status(NOT_FOUND);

  closedir(dir);
}

void ws::Repository::set_content_type() {
  if (!_autoindex.empty() || _status >= BAD_REQUEST)
    _content_type = "text/html";
  else if (_config.redirect.first > 0 && _config.redirect.first < 300)
    _content_type = "application/octet-stream";
  // nginx에서 바이너리 파일 어떤 content-type으로 주는지 확인해봐야함
  else {
    _content_type = "text";
  }
}

void ws::Repository::open_file(std::string filename) {
  if (_method == "DELETE" || _method == "HEAD")
    return;

  int open_flag;
  
  if (_method == "GET")
    open_flag = O_RDONLY;
  else
    open_flag = O_WRONLY | O_TRUNC | O_CREAT;

  if ((_fd = open(filename.c_str(), open_flag, 0644)) == -1)
    this->set_status(INTERNAL_SERVER_ERROR);
}

void ws::Repository::open_error_html() {
  error_page_map_type::const_iterator error_iter = _config.error_page_map.find(_status);
  std::string filename;
  int open_flag = O_RDONLY;

  // 기본 에러 페이지 또는 제공된 에러 페이지
  if (error_iter != _config.error_page_map.end())
    filename = error_iter->second;
  else
    filename = _index_root + "/" + ws::Util::ultos(_status) + ".html"; // _defualt_root_path + status.html

  if ((_fd = open(filename.c_str(), open_flag, 0644)) == -1)
    this->set_fatal();
}

/*getter*/
const ws::Server* ws::Repository::get_server() const throw() {
  return _server;
}

const ws::Location* ws::Repository::get_location() const throw() {
  return _location;
}

bool  ws::Repository::is_fatal() const throw() {
  return _fatal;
}

const int&  ws::Repository::get_fd() const throw() {
  return _fd;
}

const unsigned int&  ws::Repository::get_status() const throw() {
  return _status;
}

const struct stat&  ws::Repository::get_file_stat() const throw() {
  return _file_stat;
}

const std::string&  ws::Repository::get_host() const throw() {
  return _host;
}

const std::string&  ws::Repository::get_method() const throw() {
  return _method;
}

const std::string&  ws::Repository::get_root() const throw() {
  return _config.root;
}

const std::string&  ws::Repository::get_uri() const throw() {
  return _uri;
}

const std::string&  ws::Repository::get_index_root() const throw() {
  return _index_root;
}

const std::string&  ws::Repository::get_file_path() const throw() {
  return _file_path;
}

const std::string&  ws::Repository::get_request_body() const throw() {
  return _request_body;
}

const ws::Repository::autoindex_type&  ws::Repository::get_autoindex() const throw() {
  return _autoindex;
}

const ws::Repository::cgi_type&  ws::Repository::get_cgi() const throw() {
  return _cgi;
}

const std::string&  ws::Repository::get_content_type() const throw() {
  return _content_type;
}

const ws::Repository::redirect_type&  ws::Repository::get_redirect() const throw() {
  return _config.redirect;
}

void ws::Repository::clear() throw() {
  _fatal = false;
  memset(&_file_stat, 0, sizeof(struct stat));
  _status = 0;
  _fd = FD_DEFAULT;
  _index_root.clear();
  _uri.clear();
  _file_path.clear();
  _host.clear();
  _method.clear();
  _request_body.clear();
  _autoindex.clear();
  _cgi.first.clear();
  // todo: cgi second
  _content_type.clear();
  _server = NULL;
  _location = NULL;
  _request = NULL;
}

// todo: test print
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YLW "\e[0;33m"
#define CYN "\e[0;36m"

void ws::Repository::test() {
  std::cout << GRN << "\n== " << NC << "Repository" << GRN << " ==============================\n" << NC << std::endl;

  std::cout << YLW << "** Config data **" << NC << std::endl;
  std::cout << CYN << "[Type: pair]   " << NC << "- " << RED << "listen: " << NC << _config.listen.first << ", " << _config.listen.second << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "server_name: " << NC << _config.server_name << std::endl;
  std::cout << CYN << "[Type: vector] " << NC << "- " << RED << "limit_except_vec:" << NC;
  for (limit_except_vec_type::iterator it = _config.limit_except_vec.begin(); it != _config.limit_except_vec.end(); ++it)
    std::cout << " " << *it;
  std::cout << std::endl;
  std::cout << CYN << "[Type: pair]   " << NC << "- " << RED << "redirect: " << NC << _config.redirect.first << ", " << _config.redirect.second << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "cgi: " << NC << _config.cgi << std::endl;
  std::cout << CYN << "[Type: bool]   " << NC << "- " << RED << "autoindex: " << NC << _config.autoindex << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "root: " << NC << _config.root << std::endl;
  std::cout << CYN << "[Type: vector] " << NC << "- " << RED << "index:" << NC;
  for (index_set_type::iterator it = _config.index.begin(); it != _config.index.end(); ++it)
    std::cout << " " << *it;
  std::cout << std::endl;
  std::cout << CYN << "[Type: ul]     " << NC << "- " << RED << "client_max_body_size: " << NC << _config.client_max_body_size << std::endl;
  std::cout << CYN << "[Type: map]    " << NC << "- " << RED << "error_page_map:" << NC;
  for (error_page_map_type::iterator it = _config.error_page_map.begin(); it != _config.error_page_map.end(); ++it)
    std::cout << " [" << it->first << ", " << it->second << "]";
  std::cout << "\n" << std::endl;

  std::cout << YLW << "** Repository member **" << NC << std::endl;
  std::cout << CYN << "[Type: bool]   " << NC << "- " << RED << "_fatal: " << NC << _fatal << std::endl;
  std::cout << CYN << "[Type: ul]     " << NC << "- " << RED << "_status: " << NC << _status << std::endl;
  std::cout << CYN << "[Type: int]    " << NC << "- " << RED << "_fd: " << NC << _fd << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "_uri: " << NC << _uri << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "_host: " << NC << _host << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "_method: " << NC << _method << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "_request_body: " << NC << _request_body << std::endl;
  std::cout << CYN << "[Type: vector] " << NC << "- " << RED << "_autoindex:" << NC;
  for (autoindex_type::iterator it = _autoindex.begin(); it != _autoindex.end(); ++it)
    std::cout << " " << *it;
  std::cout << std::endl;
  std::cout << CYN << "[Type: pair]   " << NC << "- " << RED << "_cgi: " << NC << _cgi.first << ", " << _cgi.second << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "_content_type: " << NC << _content_type << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "_index_root: " << NC << _index_root << std::endl;

  std::cout << GRN << "\n============================================\n" << NC << std::endl;
}
