#include "Validator.hpp"
#include "Socket.hpp"

ws::Validator::Validator() {
  // _validate_func.insert(validate_func::value_type("Content-Length", &Validator::check_content_length));
  // _validate_func.insert(validate_func::value_type("Connection", &Validator::check_connection));
  // _validate_func.insert(validate_func::value_type("Transfer-Encoding", &Validator::check_transfer_encoding));
  // _validate_func.insert(validate_func::value_type("Host", &Validator::check_host));
}

ws::Validator::~Validator() {}

void ws::Validator::operator()(client_value_type* client_data) {
  check_method(client_data);
  check_uri(client_data);
  check_version(client_data);
  _validate_func.insert(validate_func::value_type("Content-Length", &Validator::check_content_length));
  _validate_func.insert(validate_func::value_type("Connection", &Validator::check_connection));
  _validate_func.insert(validate_func::value_type("Transfer-Encoding", &Validator::check_transfer_encoding));
  _validate_func.insert(validate_func::value_type("Host", &Validator::check_host));
}

void ws::Validator::check_method(client_value_type* client_data) {
  std::string method = client_data->request->get_method();

  ws::Location::limit_except_vec_type limit_except_vec = client_data->repository->get_limit_except_vec();
  for (ws::Location::limit_except_vec_type::iterator it = limit_except_vec.begin(); it != limit_except_vec.end(); ++it) {
    if (*it == method)
      return;
  }
  client_data->status = 403; //403 error : 금지된 메소드 사용
  //?? 405 error : 메소드는 허용되었지만 실패
}

void ws::Validator::check_uri(client_value_type* client_data) {
  if (client_data->request->get_uri() != client_data->repository->get_dir())
    client_data->status = 403; //403 error : 콘텐츠 접근 미승인
  return;
}

void ws::Validator::check_version(client_value_type* client_data) {
  if (client_data->request->get_version() != "HTTP/1.1")
    client_data->status = 505; // 505 error : 지원되지 않은 HTTP버전 요청받음
  return;
}

void check_content_length(const std::string&, ws::Validator::client_value_type& client_data) {
  if (client_data.request->get_content_length() != std::numeric_limits<unsigned long>::max() \
  || !(client_data.request->get_transfer_encoding() == "")) //유효하지 않은 값은 파싱단계에서 빈 문자열로 바꾸기
    client_data.status = 400;
  if (client_data.request->get_method() == "GET" \
  && (client_data.repository->get_client_max_body_size() != client_data.response_message.length()))
    client_data.status = 400;
    //GET 메소드를 사용한 경우 응답의 페이로드 본문에 전송 된 10진수와 Content-Length의 field-value가 같지 않는 한, 서버는 응답으로 Content-Length를 보내서는 안 된다
  if (client_data.status == 204 || (client_data.status / 100 <= 1 && client_data.status != 0))
    client_data.status = 400;
    //*상태 코드가 1xx (Informational) 또는 204 (No Content)인 응답에서 Content-Length헤더 필드를 보내면 안 된다.(MUST NOT) 
  return;
}

void check_connection(const std::string&, ws::Validator::client_value_type& client_data) {
  if (client_data.request->get_connection() == "close") {

  } else { //default: "keep-alive"

  }
}

void check_transfer_encoding(const std::string&, ws::Validator::client_value_type& client_data) {
  //chunked -> request _chunked setting
  if (client_data.request->get_transfer_encoding() == "chunked") {
    if (client_data.request->get_content_length() == std::numeric_limits<unsigned long>::max())
      client_data.status = 400; //2번검사?
  } else {

  }
}

void check_host(const std::string&, ws::Validator::client_value_type& client_data) {
  std::string host = client_data.request->get_server_name();
  if (host == "") //비어있기, 유요하지않을때, 두개이상의 host
    client_data.status = 400;
}
