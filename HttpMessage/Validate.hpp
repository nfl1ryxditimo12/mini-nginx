#include "RequestMessage.hpp"

namespace ws {
	class validate {
	private:
		const ws::Configure* _conf;

	public:
		typedef ws::RequestMessage::header_type header_type;
		typedef header_type::iterator	header_iterator;

		validate(const ws::Configure* conf) : _conf(conf) {}

		void check_method(u_int32_t host, const std::string& dir, const std::string& method);
		void check_version(const std::string& _http_version);
		void check_transfer_encoding();
	};	
}
