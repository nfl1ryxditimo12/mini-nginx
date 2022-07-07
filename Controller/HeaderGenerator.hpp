#pragma once

#include <string>

namespace ws {
  class HeaderGenerator {
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

    HeaderGenerator(const HeaderGenerator& other);
    HeaderGenerator& operator=(const HeaderGenerator& other);

  public:
    HeaderGenerator();
    ~HeaderGenerator();

    const std::string& get_data() const throw();

    void generate_data(unsigned int stat, std::string::size_type content_length);

    // test function
    void print_data() const;
  };
}
