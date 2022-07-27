#include "Repository.hpp"
#include "Request.hpp"

ws::Repository::Repository(bool fatal, unsigned int status): _fatal(fatal), _status(status), _session(0), _fd(FD_DEFAULT) {
  memset(&_file_stat, 0, sizeof(struct stat));
  _index_root = ws::Util::get_root_dir() + "/www";
}

ws::Repository::Repository(const Repository& cls): _fatal(cls._fatal), _status(cls._status) {
  _session = cls._session;
  _fd = cls._fd;
  _uri = cls._uri;
  _file_path = cls._file_path;
  _host = cls._host;
  _method = cls._method;
  _request_body = cls._request_body;
  _autoindex = cls._autoindex;
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
    if (_location->get_block_name() == "/session" && _request->get_uri() == "/session")
      _session = _location->get_session();
    _config.limit_except_vec = _location->get_limit_except_vec();
    _config.redirect = _location->get_return();
/*set option*/
    ws::Repository::set_option(_location->get_option());
  } else
    ws::Repository::set_option(_server->get_option());

  _file_path = _config.root + (_uri[0] == '/' || !_uri.length() ? "" : "/") + _uri;

  _file_exist_stat = lstat(_file_path.c_str(), &_file_stat) != -1;

  std::string server_name = _request->get_server_name() == "_" ? "localhost" : _request->get_server_name();

  _host = server_name + ":" + ws::Util::ultos(ntohs(_config.listen.second));
  _method = _request->get_method();
}

void ws::Repository::set_option(const ws::InnerOption& option) {
  _config.autoindex = option.get_autoindex();
  _config.root = option.get_root();
  _config.index = option.get_index_set();
  _config.index_check = option.get_index_check();
  _config.client_max_body_size = option.get_client_max_body_size();
  _config.error_page_map = option.get_error_page_map();
}

void ws::Repository::set_repository(unsigned int value)  {
  this->set_status(value);
  if (_config.redirect.first > 0)
    this->set_status(_config.redirect.first); // todo

  if (_status == 0 && !_session) {
    if (_file_exist_stat && S_ISDIR(_file_stat.st_mode))
      this->set_autoindex();
    else if (!_file_exist_stat && !(_method == "POST" || _method == "PUT"))
      _status = NOT_FOUND;
    else if ((_method == "GET" || _method == "HEAD") && !(_file_stat.st_mode & S_IRUSR))
      _status = FORBIDDEN;
    else if ((_method == "POST" || _method == "PUT") && !(_file_stat.st_mode & S_IWUSR))
      _status = FORBIDDEN;
    else
      this->open_file(_file_path);
  }
  
  if (_status >= BAD_REQUEST)
    this->open_error_html();

  if (_status == 0)
    this->set_status(OK); // todo: Put, Post status is originally 201

  this->set_content_type();
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
  DIR* dir = opendir(_file_path.c_str());
  struct dirent *file    = NULL;

  if (dir == NULL)
    this->set_status(INTERNAL_SERVER_ERROR);

  std::cout << _config.index_check << std::endl;

  while (dir && ((file = readdir(dir)) != NULL)) {
    index_set_type::const_iterator filename = _config.index.find(file->d_name);

    if (filename != _config.index.end() && (_config.index_check || (!_config.index_check && !_config.autoindex))) {
      open_file(_file_path + "/" + *filename);
      _autoindex.clear();
      break;
    }

    if (_config.autoindex)
      _autoindex.push_back(file->d_name);
  }

  if (_fd == FD_DEFAULT && !(_method == "GET" || _method == "HEAD")) {
    _autoindex.clear();
    this->set_status(METHOD_NOT_ALLOWED);
  } else if (_fd == FD_DEFAULT && !_config.autoindex && !(_method == "HEAD" || _method == "DELETE"))
    this->set_status(NOT_FOUND);

  if (dir)
    closedir(dir);
}

void ws::Repository::set_content_type() {
  if (_method != "GET")
    _content_type = "text";
  else if (!_autoindex.empty() || _status >= BAD_REQUEST)
    _content_type = "text/html";
  else if (_config.redirect.first > 0 && _config.redirect.first < 300)
    _content_type = "application/octet-stream";
  else {
    std::string::size_type pos = _uri.find_last_of('.');
    std::string extension = pos != std::string::npos ?  _uri.substr(pos + 1) : "";
    _content_type = ws::Util::mime_type(extension);
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

  if ((_fd = open(filename.c_str(), open_flag, 0644)) == -1 || fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
    if (errno == ENOENT)
      set_status(NOT_FOUND);
    this->set_status(INTERNAL_SERVER_ERROR);
  }
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

  if ((_fd = open(filename.c_str(), open_flag, 0644)) == -1 || fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
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

bool ws::Repository::is_session() const throw() {
  return _session;
}

const struct stat&  ws::Repository::get_file_stat() const throw() {
  return _file_stat;
}

bool ws::Repository::get_file_exist_stat() const throw() {
  return _file_exist_stat;
}

const std::string&  ws::Repository::get_content_type() const throw() {
  return _content_type;
}

const std::string&  ws::Repository::get_method() const throw() {
  return _method;
}

const std::string&  ws::Repository::get_file_path() const throw() {
  return _file_path;
}

const ws::Repository::autoindex_type&  ws::Repository::get_autoindex() const throw() {
  return _autoindex;
}

const ws::Repository::redirect_type&  ws::Repository::get_redirect() const throw() {
  return _config.redirect;
}

const ws::Repository::client_max_body_size_type& ws::Repository::get_client_max_body_size() const throw() {
  return _config.client_max_body_size;
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
  _content_type.clear();
  _server = NULL;
  _location = NULL;
  _request = NULL;
}
