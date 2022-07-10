#include "Repository.hpp"
#include "Request.hpp"

/*
  todo

  fatal 인 경우 전체적인 흐름 유지하는 처리

  테스트 하며 멤버 콘솔 출력 // seonkim
  _root + _uri 중간에 슬래시 붙는지 확인해야함
  파일 오픈 분기 delete 처리 해줘야함
  권한 없을 때 파일 열기 어떻게 처리할지
*/

ws::Repository::Repository(bool fatal, unsigned int status): _fatal(fatal), _status(status), _fd(FD_DEFAULT) {
  _project_root = ws::Util::get_root_dir();

}

ws::Repository::Repository(const Repository& cls): _fatal(cls._fatal), _status(cls._status) {
  _fd = cls._fd;
  _uri = cls._uri;
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
  _project_root = cls._project_root;
}

ws::Repository::~Repository() {}

void ws::Repository::operator()(const ws::Server& server, const ws::Request& request) {
  _uri = request.get_uri();
  std::string file = _project_root + _uri;
  
  _server = &server;
  _location = &(_server->find_location(file + _uri));
  _request = &request;
  _request_body = _request->get_request_body();


  lstat(file.c_str(), &_file_stat);

  /*set server*/
  _config.listen = request.get_listen();
  _config.server_name = request.get_request_header().find("Host")->second;
  _config.server_name = request.get_server_name();

  if (_location != NULL) {
    _config.limit_except_vec = _location->get_limit_except_vec();
    _config.redirect = _location->get_return();
    _config.cgi = _location->get_cgi();
  /*set option*/
    ws::Repository::set_option(_location->get_option());
  } else
    ws::Repository::set_option(_server->get_option());
}

void ws::Repository::set_option(const ws::InnerOption& option) {
  _config.autoindex = option.get_autoindex();
  _config.root = option.get_root();
  _config.index = option.get_index_vec();
  _config.client_max_body_size = option.get_client_max_body_size();
  _config.error_page_map = option.get_error_page_map();
}

void ws::Repository::set_repository(unsigned int status)  {
  std::string server_name = _request->get_server_name() == "_" ? "localhost" : _request->get_server_name();

  this->set_status(status);
  if (_config.redirect.first > 0)
    this->set_status(_config.redirect.first); // todo

  _host = server_name + ":" + ws::Util::ultos(ntohs((_config.listen.second)));
  _method = _request->get_method();

  // file_stat 초기화 해줘야함


  if (S_ISDIR(_file_stat.st_mode) && !_config.redirect.first)
    this->set_autoindex();
  else if (status == 0)
    open_file(_config.root + _uri);

  if (status >= BAD_REQUEST)
    open_error_html();

  if (!_status)
    set_status(_method == "POST" ? 201 : 200);

  set_content_type();
  
  // 지울거임
  test();
}

void ws::Repository::set_status(unsigned int status) {
  if (_status >= BAD_REQUEST)
    return;
  _status = status;
}

void ws::Repository::set_fatal() {
  _fatal = true;
}

void ws::Repository::set_autoindex() {
  std::string path = _project_root + _uri;
  DIR* dir = opendir(path.c_str());
  struct dirent *file    = NULL;

  if (dir == NULL)
    this->set_status(INTERNAL_SERVER_ERROR);

  while (dir && ((file = readdir(dir)) != NULL)) {
    std::string filename(file->d_name);

    /* index.html은 기본값이고 index_vec_type -> index_set_type 으로 수정될 예정 */
    for (index_vec_type::const_iterator it = _config.index.begin(); it != _config.index.end(); ++it) {
      if (filename == *it) {
        open_file(_config.root + filename);
        break;
      }
    }

    if (_fd != FD_DEFAULT) {
      _autoindex.clear();
      break;
    }

    if (_config.autoindex)
      _autoindex.push_back(filename);
  }

  if (_fd == FD_DEFAULT && !_config.autoindex)
    this->set_status(403);

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
  int open_flag = _method == "GET" ? O_RDONLY : O_WRONLY | O_TRUNC | O_CREAT;

//  if ((_fd = open(filename.c_str(), open_flag, 644)) == -1)
  if ((_fd = open(filename.c_str(), open_flag, 644)) == -1)
    this->set_status(INTERNAL_SERVER_ERROR);
}

void ws::Repository::open_error_html() {
  error_page_map_type::const_iterator error_iter = _config.error_page_map.find(_status);
  std::string filename;
  int open_flag = O_WRONLY | O_TRUNC | O_CREAT;

  // 기본 에러 페이지 또는 제공된 에러 페이지
  if (error_iter != _config.error_page_map.end())
    filename = error_iter->second;
  else
    filename = _project_root + "/" + ws::Util::ultos(_status) + ".html"; // _defualt_root_path + status.html

  if ((_fd = open(filename.c_str(), open_flag, 644)) == -1)
    this->set_fatal();
  
}

/*getter*/
const ws::Server* ws::Repository::get_server() const throw() {
  return _server;
}

const ws::Location* ws::Repository::get_location() const throw() {
  return _location;
}

bool  ws::Repository::get_fatal() const throw() {
  return _fatal;
}

const int&  ws::Repository::get_fd() const throw() {
  return _fd;
}

const unsigned int&  ws::Repository::get_status() const throw() {
  return _status;
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

#include <iostream>
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
  for (index_vec_type::iterator it = _config.index.begin(); it != _config.index.end(); ++it)
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
  for (index_vec_type::iterator it = _autoindex.begin(); it != _autoindex.end(); ++it)
    std::cout << " " << *it;
  std::cout << std::endl;
  std::cout << CYN << "[Type: pair]   " << NC << "- " << RED << "_cgi: " << NC << _cgi.first << ", " << _cgi.second << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "_content_type: " << NC << _content_type << std::endl;
  std::cout << CYN << "[Type: string] " << NC << "- " << RED << "_project_root: " << NC << _project_root << std::endl;

  std::cout << GRN << "\n============================================\n" << NC << std::endl;
}
