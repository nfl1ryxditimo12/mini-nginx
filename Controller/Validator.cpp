#include "Validator.hpp"

ws::Validator::Validator(const ws::Configure& _conf, const RequestMessage& _request) {}

ws::Validator::~Validator() {}

bool ws::Validator::check_method(ws::Configure* _conf, ws::RequestMessage* _request) {
  std::string method = _request->get_method();
  ws::Location::limit_except_type limit_except_vec = _conf->get_server_vec().get_location_map().get_limit_except_vec();
 
  for (ws::Location::limit_except_vec_type::iterator it = limit_except_vec.begin(); it != limit_except_vec.end(); ++it) {
    if (*it == method)
      return true;
  }
  throw; //403 error : 금지된 메소드 사용
  //?? 405 error : 메소드는 허용되었지만 실패
}

bool ws::Validator::check_uri(ws::Configure* _conf, ws::RequestMessage* _request) {
  std::string _request_uri = _request->get_uri();
  //location dir
  throw; //403 error : 콘텐츠 접근 미승인
}

bool ws::Validator::check_version(ws::RequestMessage* _request) {
  std::string _http_version = _request->get_version();
  if (_http_version.find("HTTP") == std::string::npos)
    throw; // not http protocol
  if (_http_version != "HTTP/1.1")
    throw; // 505 error : 지원되지 않은 HTTP버전 요청받음
}

bool ws::Validator::check_request_header(ws::Configure* _conf, ws::RequestMessage* _request) {
  for (ws::RequestMessage::header_type::iterator header_iter = _request->get_request_header().begin(); header_iter != _request->get_request_header().end() ; header_iter++) {
    if (header_iter->first == "Content-Length") {
      if (ws::Util::stoul(header_iter->second) != _request->get_request_body().length())
        throw;
      //@ 411error : (POST요청에) content-length가 없음
    }
    if (header_iter->first == "Content_type") {
      //text/plain only //indextype?
    }
    if (header_iter->first == "Connection") {
      //keep-alive only //close?
    }
    if (header_iter->first == "Transfer-Encoding") {
      if (header_iter->second != "chunked")
        throw;
    }
    if (header_iter->first == "Host") {
      //conf의 server name이랑 http request header의 host값으로 어떤 서버블록인지 확인
      for (ws::Configure::server_vec_type::iterator it = _conf->get_server_vec().begin(); _conf->get_server_vec().end(); it++) {
        for (ws::Server::server_name_vec_type::iterator name_iter = it->get_server_name_vec().begin(); it->get_server_name_vec().end(); name_iter++) {
          if (header_iter->second == *name_iter)
            // ??
        }
      }
    }
  }
}
