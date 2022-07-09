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

ws::Repository::Repository(bool& fatal, unsigned int& status): _fatal(fatal), _status(status), _fd(FD_DEFAULT) {}

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
  _proejct_root = cls._proejct_root;
  _return = cls._return;
}

ws::Repository::~Repository() {}

void ws::Repository::operator()(const ws::Server* server, const ws::Request* request) {
  _server = server;
  _location = _server->find_location(request->get_uri());
  _request = request;
  /*set server*/
  _config.listen = request->get_listen();
  _config.server_name = request->get_request_header().find("Host")->second;
  // _server_name = request.get_server_name();

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
  const std::string& server_name = _request->get_server_name() == "_" ? "localhost" : _request->get_server_name();
  struct stat file_stat;

  this->set_status(status);
  if (_config.redirect.first > 0)
    this->set_status(_config.redirect.first);

  _host = server_name + ":" + ws::ultoa(ntohs((_config.listen.second)));
  _method = _request->get_method();

  if (S_ISDIR(file_stat.st_mode)) {
    if (_config.autoindex || _config.index.size())
      this->set_autoindex();
  }

  if (_config.redirect.first > 400 || _fd == FD_DEFAULT || _autoindex.empty())
    open_file(_config.root + _uri);

  set_content_type();
}

void ws::Repository::set_status(const unsigned int status) {
  if (_status >= BAD_REQUEST)
    return;
  _status = status;
}

void ws::Repository::set_autoindex() {
  DIR* dir = opendir(_config.root.c_str());
  struct dirent *file    = NULL;

  if (dir == NULL)
    this->set_status(INTERNAL_SERVER_ERROR);

  while (dir && ((file = readdir(dir)) != NULL)) {
    std::string filename(file->d_name);

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

    _autoindex.push_back(filename);
  }

  closedir(dir);
}

void ws::Repository::set_content_type() {
  if (!_autoindex.empty() || _status >= BAD_REQUEST) {
    _content_type = "text/html";
  }
  else if (_config.redirect.first == static_cast<const unsigned int>(_status)) {
    if (_status < 300)
      _content_type = "application/octet-stream";
  }
  // nginx에서 바이너리 파일 어떤 content-type으로 주는지 확인해봐야함
  else {
    _content_type = "text";
  }
}

void ws::Repository::open_file(std::string filename) {
  error_page_map_type::const_iterator error_iter = _config.error_page_map.find(_status);
  int open_flag = _method == "GET" ? O_RDONLY : O_WRONLY | O_TRUNC | O_CREAT;

  // 기본 에러 페이지 또는 제공된 에러 페이지
  if (_status >= BAD_REQUEST) {
    if (error_iter != _config.error_page_map.end())
      filename = error_iter->second;
    else
      filename = ""; // _defualt_root_path + status.html
  }

  if ((_fd = open(filename.c_str(), open_flag, 644)) == -1)
    throw; // 프로세스 종료해야함
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

const int&  ws::Repository::get_status() const throw() {
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
