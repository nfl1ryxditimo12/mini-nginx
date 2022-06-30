#include "RequestMessage.hpp"
#include "Configure.hpp"
#include "Util.hpp"

namespace ws {
	class Validator {
	public:
		// typedef ws::RequestMessage::header_type header_type_;
		// typedef header_type_::iterator	header_iterator;

	private:
		const ws::Configure _conf;
		const ws::RequestMessage _request;
		// const ws::Server _server;

	public:
		Validator(const ws::Configure& conf, const RequestMessage& request);
		~Validator();

		bool check_method(ws::Configure* _conf, ws::RequestMessage* _request);
		bool check_uri(ws::Configure* _conf, ws::RequestMessage* _request);
		bool check_version(ws::RequestMessage* _request);

		bool check_request_header(ws::Configure* _conf, ws::RequestMessage* _request);

	};	
}
