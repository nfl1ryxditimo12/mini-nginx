#include "RequestMessage.hpp"

namespace ws {
	class validate {
	public:
		typedef ws::RequestMessage::header_type header_type_;
		typedef header_type_::iterator	header_iterator;

	private:
		const ws::Configure* _conf;

	public:
		validate(const ws::Configure* conf) : _conf(conf) {}

		void check_method(u_int32_t host, const std::string& dir, const std::string& method);
		void check_uri(const std::string _request_uri);
		void check_version(const std::string& _http_version);
		void check_request_header(header_type_& _request_header);
		void check_content_length();
		void check_transfer_encoding();
	};	
}
