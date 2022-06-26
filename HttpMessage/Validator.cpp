#include "Validator.hpp"

ws::Validator::Validator(const ws::Configure *conf) {}

ws::Validator::~Validator() {}

void ws::Validator::check_method(u_int32_t host, const std::string& dir, const std::string& method) {
  header_iterator iter;
  iter = _conf->get_server(host).get_location(dir).get_limite_except().find(method);
  if (iter == npos || iter->second == false)
    throw; //403 error : 금지된 메소드 사용
  // 405 error : 메소드는 허용되었지만 실패
}

void check_uri(const std::string _request_uri) {
  // int split;
  // if (split = _request_uri.find("?") == std::string::npos)
  // 	return;
  // _request_uri_query = _request_uri.substr(split + 1);
  // _request_uri = _request_uri.substr(0, split);

  if (_request_uri == "" || _request_uri[0] != '/')
    throw; //403 error : 콘텐츠 접근 미승인
}

void check_version(const  std::string& _http_version) {
  if (_http_version.find("HTTP") == std::string::npos)
    throw; //not http protocol
  if (_http_version != "HTTP/1.1")
    throw; //505 error : 지원되지 않은 HTTP버전 요청받음
}

bool ws::Validator::check_empty_value(std::string& second) {
  if (second.length())
    return true;
  return false;
}

void ws::Validator::check_request_header(header_type_& _request_header) {
  for (header_iterator header_iter = _request_header.begin(); header_iter != _request_header.end() ; header_iter++) {
    check_empty_value(header_iter->second);
    if (header_iter->first == "Content-Length") {
      if (stoi(header_iter->second) != _request_message.get_request_body().length()) //stoi
        throw;
      //@ 411error : (POST요청에) content-length가 없음
    }
    if (header_iter->first == "Connection") {
      //keep-alive, close
    }
    if (header_iter->first == "Content_type") {
      //text/plain
    }
    if (header_iter->first == "Transfer-Encoding") {
      if (header_iter->second != "chunked")
        throw;
    }
    if (header_iter->first == "Host") {
      int split;
      std::string host;
      int port;

      if (split = header_iter->second.find(":") == std::string::npos) {
        port = 80;
        host = header_iter->second;
      } else {
        host = header_iter->second.substr(0, split);
        port = stoi(header_iter->second.substr(split + 1));
      }
      // if (_server.get_server_name().find(host) == npos)
      //   throw;
      // if (_server.get_listen()->second.find(port) == npos)
      //   throw;

      //1-1. string: localhost -> servername 
      // get_server_name()vector ->string == host
      for (ws::Server::server_name_type::iterator server_name_iter = _server.get_server_name().begin(); server_name_iter != _server.get_server_name().end(); server_name_iter++) {
        if (server_name_iter == host) {
          //1-2. int: listen-ip 
          // inet_addr(host) == get_listen()vector->pair ->first
          for (ws::Server::listen_type::iterator listen_iter = _server.get_listen().begin(); listen_iter != _server.get_listen().end() ; listen_iter++) {
            if (listen_iter->second == port)
              break;
          }
        }
        throw;
      }
    }
  }
}
