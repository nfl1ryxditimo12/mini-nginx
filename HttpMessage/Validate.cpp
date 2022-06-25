#include "Validate.hpp"

void check_method(u_int32_t host, const std::string& dir, const std::string& method) {
	header_iterator iter;
	iter = _conf->get_server(host).get_location(dir).get_limite_except().find(method);
	if (iter == npos || iter->second == false)
		throw; //403 error : 금지된 메소드 사용
	// 405 error : 메소드는 허용되었지만 실패
}

// void check_uri(const std::string& _request_uri)

void check_version(const std::string& _http_version) {
	if (_http_version != "HTTP/1.1")
		throw; //505 error : 지원되지 않은 HTTP버전 요청받음
}

// void check_content_length() {
// 	if (_body.length() != get_content_length())
// 		throw;
// }

// transfer_encoding이 chunked가 아니면 오류?/
void check_transfer_encoding() {
	header_type::iterator header_iter = _request_header.find("Transfer-Encoding");
	if (header_iter == _request_header.end() || _request_header["Transfer-Encoding"] != "chunked"))
		throw;
}

//date에 gettime해주기 -> 시간으로바꾸기

// 1. map에 저장할 때 필수 속성이 안들어오면 알맞은 에러 반환
// 2. 정상적으로 저장되었다면 각 속성들에 이상한 문법으로 들어와있는지 확인
// 2-1. 없어도 되는 속성이면 초기화값이어도 오류가 아님








	// if (!check_method(_method, ))
		// throw; //403 error : 금지된 메소드 사용


	// if (!check_uri(_request_uri))
	// 	throw; //403 error : 콘텐츠 접근 미승인


	// if (_http_version != "HTTP/1.1")
		// throw; 




	//transfer-encoding이 chunked -> chunked body
	// for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++) {
	//content-length
		// if (it->first == "Content-Length")
		// 	if (!check_content_length(it->second, _body.length()))
		// 		throw; //411error : (POST요청에) content-length가 없음
	// ...
