#include "RequestMessage.hpp"
#include "Configure.hpp"
#include "Server.hpp"

namespace ws {
	class Validator {
	public:
		typedef ws::RequestMessage::header_type header_type_;
		typedef header_type_::iterator	header_iterator;

	private:
		const ws::Configure _conf;
		const ws::RequestMessage _request_message;
		const ws::Server _server;

	public:
		Validator(const ws::Configure& conf, const RequestMessage& request);
		~Validator();

		void check_method(u_int32_t host, const std::string& dir, const std::string& method);
		void check_uri(const std::string _request_uri);
		void check_version(const std::string& _http_version);
		
		void check_request_header(header_type_& _request_header);
		bool check_empty_value(std::string& second);
	};	
}
