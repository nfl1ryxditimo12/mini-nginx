#include "Validator.hpp"

ws::Validator::Validator() {
  _validate_func.insert(validate_func::value_type("Content-Length", &Validator::check_content_length));
  _validate_func.insert(validate_func::value_type("Connection", &Validator::check_connection));
  _validate_func.insert(validate_func::value_type("Content_type", &Validator::check_content_type));
  _validate_func.insert(validate_func::value_type("Transfer-Encoding", &Validator::check_transger_encoding));
  _validate_func.insert(validate_func::value_type("Host", &Validator::check_host));
}

ws::Validator::~Validator() {}

void ws::Validator::operator()(client_value_type* client_data) {
  _validate_func.insert(validate_func::value_type("Content-Length", &Validator::check_content_length));
  _validate_func.insert(validate_func::value_type("Connection", &Validator::check_connection));
  _validate_func.insert(validate_func::value_type("Content_type", &Validator::check_content_type));
  _validate_func.insert(validate_func::value_type("Transfer-Encoding", &Validator::check_transger_encoding));
  _validate_func.insert(validate_func::value_type("Host", &Validator::check_host));
}

// void ws::Validator::check_method() {
//   std::string method = _request.get_method();

//   ws::Location::limit_except_vec_type limit_except_vec = _repository.get_limit_except_vec();
//   for (ws::Location::limit_except_vec_type::iterator it = limit_except_vec.begin(); it != limit_except_vec.end(); ++it) {
//     if (*it == method)
//       return;
//   }
//   throw; //403 error : 금지된 메소드 사용
//   //?? 405 error : 메소드는 허용되었지만 실패
// }

// void ws::Validator::check_uri() {
//   if (_request.get_uri() != _repository.get_dir())
//     throw; //403 error : 콘텐츠 접근 미승인
//   return;
// }

// void ws::Validator::check_version() {
//   if (_request.get_version() != "HTTP/1.1")
//     throw; // 505 error : 지원되지 않은 HTTP버전 요청받음
//   return;
// }

// void ws::Validator::check_request_header() {
//   for (
//     header_type::const_iterator header_iter = _request.get_request_header().begin();
//     header_iter != _request.get_request_header().end();
//     header_iter++) {
//       validate_func::iterator valid_iter = _validate_func.find(header_iter->first);

//       if (valid_iter == _validate_func.end())
//         continue;

//       (this->*valid_iter->second)(header_iter->second);

//     }
// }
