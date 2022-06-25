#include "Validate.hpp"

void check_method(u_int32_t host, const std::string& dir, const std::string& method) {
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

// void check_request_header(header_type_& _request_header) {
// 	for (header_iterator header_iter = _request_header.begin(); ; header_iter++) {
// 		if (header_iter->second == "")
// 			throw; //key가 없는 경우는 파싱에서 실패
// 		//음...	
// 	}
// }

void check_content_length() {
	header_iterator header_iter = _request_header.find("Content-Length");
	if (header_iter == _request_header.end() || _request_header["Content-Length"] != _request_body.length()))
		throw;
	if (_method == "POST" && _request_header["Content-Length"] == 0)
		throw; //411error : (POST요청에) content-length가 없음
}

void check_transfer_encoding() {
	header_iterator header_iter = _request_header.find("Transfer-Encoding");
	if (header_iter == _request_header.end() || _request_header["Transfer-Encoding"] != "chunked"))
		throw;
}
