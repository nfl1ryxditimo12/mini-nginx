#include "Validator.hpp"

ws::Validator::Validator(const Request& request, const ws::Repository& repository) {}

ws::Validator::~Validator() {}

void ws::Validator::check_method(ws::Request& request, const ws::Repository& repository) {
  std::string method = request.get_method();

  ws::Location::limit_except_vec_type limit_except_vec = repository.get_limit_except_vec();
  for (ws::Location::limit_except_vec_type::iterator it = limit_except_vec.begin(); it != limit_except_vec.end(); ++it) {
    if (*it == method)
      return ;
  }
  throw; //403 error : 금지된 메소드 사용
  //?? 405 error : 메소드는 허용되었지만 실패
}

void ws::Validator::check_uri(ws::Request& request, const ws::Repository& repository) {
  if (request.get_uri() != repository.get_dir())
    throw; //403 error : 콘텐츠 접근 미승인
  return ;
}

void ws::Validator::check_version(ws::Request& request, const ws::Repository& repository) {
  if (request.get_version().find("HTTP") == std::string::npos)
    throw; // not http protocol
  if (request.get_version() != "HTTP/1.1")
    throw; // 505 error : 지원되지 않은 HTTP버전 요청받음
  return ;
}

void ws::Validator::check_request_header(ws::Request& request, const ws::Repository& repository) {
  for (
    ws::Request::header_type::const_iterator header_iter = request.get_request_header().begin();
    header_iter != request.get_request_header().end();
    header_iter++) 
    {
  //   if (header_iter->first == "Content-Length") {
  //     if (ws::stoul(header_iter->second) != repository.get_request_body().length())
  //       throw;
  //     //@ 411error : (POST요청에) content-length가 없음
  //   }
  //   if (header_iter->first == "Connection") {
  //     //keep-alive only //close?
  //   }
  //   if (header_iter->first == "Content_type") {
  //     //text/plain only //indextype?
  //   }
  //   if (header_iter->first == "Transfer-Encoding") {
  //     if (header_iter->second != "chunked")
  //       throw;
  //   }
  //   if (header_iter->first == "Host") {
  //     //conf의 server name이랑 http request header의 host값으로 어떤 서버블록인지 확인
  //     for (ws::Configure::server_vec_type::iterator it = conf->get_server_vec().begin(); conf->get_server_vec().end(); it++) {
  //       for (ws::Server::server_name_vec_type::iterator name_iter = it->get_server_name_vec().begin(); it->get_server_name_vec().end(); name_iter++) {
  //         if (header_iter->second == *name_iter)
  //           // ??
  //       }
  //     }
  //   }
  // }
}
