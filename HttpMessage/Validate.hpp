#include "RequestMessage.hpp"

namespace ws {
	class validate {
	private:
		const ws::Configure* _conf;

	public:
		typedef ws::RequestMessage::header_type header_type;
		typedef header_type::iterator	header_iterator;

		validate(const ws::Configure* conf) : _conf(conf) {}

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

		// void check_transfer_encoding() {
			// transfer_encoding이 chunked가 아니면 오류?
		// }

		//date에 gettime해주기 -> 시간으로바꾸기
		
		// 1. map에 저장할 때 필수 속성이 안들어오면 알맞은 에러 반환
		// 2. 정상적으로 저장되었다면 각 속성들에 이상한 문법으로 들어와있는지 확인
		// 2-1. 없어도 되는 속성이면 초기화값이어도 오류가 아님

	};	
}
