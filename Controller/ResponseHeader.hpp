#pragma once

#include <string>

namespace ws {
  class ResponseHeader {
  private:
    std::string _data;

    // static functions
    void generate_start_line(unsigned int stat);
    void generate_representation_header(unsigned int stat, std::string::size_type content_length);
    void generate_response_header();
    void generate_content_type_line();
    void generate_content_length(std::string::size_type content_length);
    void generate_date_line();
    void generate_server_line();

    ResponseHeader(const ResponseHeader& other);
    ResponseHeader& operator=(const ResponseHeader& other);

  public:
    ResponseHeader();
    ~ResponseHeader();

    const std::string& get_data() const throw();

    void generate_data(unsigned int stat, std::string::size_type content_length);
  };
}
