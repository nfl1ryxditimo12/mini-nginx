#include "Request.hpp"
#include "Repository.hpp"
#include "Configure.hpp"
#include "Util.hpp"

namespace ws {
  class Validator {
  public:
    // typedef ws::Request::header_type header_type_;
    // typedef header_type_::iterator	header_iterator;

  private:
    const ws::Configure _conf;
    const ws::Request _request;
    // const ws::Server _server;

    Validator();
    
  public:
    Validator(const Request& request, const ws::Repository& repository);
    ~Validator();

    void check_method(ws::Request& request, const ws::Repository& repository);
    void check_uri(ws::Request& request, const ws::Repository& repository);
    void check_version(ws::Request& request, const ws::Repository& repository);


    // void ws::Validator::check_const_length();
    // void ws::Validator::check_connection();
    // void ws::Validator::check_content_type();
    // void ws::Validator::check_transger_encoding();
    // void ws::Validator::check_host();
    void check_request_header(ws::Request& request, const ws::Repository& repository);

  };
}
