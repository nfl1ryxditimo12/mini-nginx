#include "Repository.hpp"
#include "Request.hpp"

/*
  vector<string> dir; //
  int file_fd;
  pair<string, char**> cgi: default = ""

  string content-type;

  그외 필요한 데이터 만들어야함
*/

ws::Repository::Repository(): _fd(FD_DEFAULT), _status(0) {}

ws::Repository::~Repository() {}

void ws::Repository::operator()(const ws::Server* server, const ws::Request* request) {
  _server = server;
  _location = _server->find_location(request->get_uri());
  _request = request;
  /*set server*/
  _config.listen = &(request->get_listen());
  _config.server_name = &(request->get_request_header().find("Host")->second);
  // _server_name = &(request.get_server_name());

  if (_location != NULL) {
    _config.limit_except_vec = &(_location->get_limit_except_vec());
    _config.redirect = &(_location->get_return());
    _config.cgi = &(_location->get_cgi());
  /*set option*/
    ws::Repository::set_option(_location->get_option());
  }
  else
    ws::Repository::set_option(_server->get_option());
}

void ws::Repository::set_option(const ws::InnerOption& option) {
  _config.autoindex = &(option.get_autoindex());
  _config.root = &(option.get_root());
  _config.index = &(option.get_index_vec());
  _config.client_max_body_size = &(option.get_client_max_body_size());
  _config.error_page_map = &(option.get_error_page_map());
}

void ws::Repository::set_repository(int status)  {
  const std::string& server_name = _request->get_server_name() == "_" ? "localhost" : _request->get_server_name();
  struct stat file_stat;

  this->set_status(status);
  this->set_status(_config.redirect->first);

  _host = server_name + ":" + ws::ultoa(ntohs((_config.listen->second)));
  _method = _request->get_method();

  if (S_ISDIR(file_stat.st_mode)) {
    if (*_config.autoindex || _config.index->size())
      this->set_autoindex();
  }
  
  /* _return < 400 이고 몰라 */
  if (_config.redirect->first != 0 && (_status >= BAD_REQUEST || _fd == FD_DEFAULT || _autoindex.empty()))
    open_file(*_config.root);

  set_content_type();
}

void ws::Repository::set_status(const int& status) {
  if (_status >= BAD_REQUEST)
    return;
  _status = status;
}

void ws::Repository::set_autoindex() {
  DIR* dir = opendir(_root.c_str());
  struct dirent *file    = NULL;

  if (dir == NULL)
    this->set_status(INTERNAL_SERVER_ERROR);

  while (dir && ((file = readdir(dir)) != NULL)) {
    std::string filename(file->d_name);

    for (index_vec_type::const_iterator it = _config.index->begin(); it != _config.index->end(); ++it) {
      if (filename == *it) {
        std::string filepath = _root + filename;
        open_file(filepath);
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
  else if (_config.redirect->first == _status) {
    if (_status < 300)
      _content_type = "application/octet-stream";
  }
  // nginx에서 바이너리 파일 어떤 content-type으로 주는지 확인해봐야함
  else {
    _content_type = "text";
  }
}

void ws::Repository::open_file(std::string filename) {
  error_page_map_type::const_iterator error_iter = _config.error_page_map->find(_status);

  // 기본 에러 페이지 또는 제공된 에러 페이지
  if (_status >= BAD_REQUEST) {
    if (error_iter != _config.error_page_map->end())
      filename = error_iter->second;
    else
      filename = ""; // _defualt_root_path + status.html
  }

  if ((_fd = open(filename.c_str(), O_RDWR, 644)) == -1)
    throw; // 프로세스 종료해야함
}

/*getter*/
const ws::Server* ws::Repository::get_server() const throw() {
  return _server;
}

const ws::Location* ws::Repository::get_location() const throw() {
  return _location;
}
