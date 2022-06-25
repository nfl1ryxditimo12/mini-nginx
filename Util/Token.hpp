#pragma once

#include <istream>
#include <string>

namespace ws {
  /*
  Token class for parsing
  By calling rdword(), Token extracts string form istream without whitespace
  Any whitespace(` ', `\t') in buffer is ignored
  */
  class Token : public std::string {
  private:
    Token& operator=(const Token& other);
    Token(const Token& other);

  public:
    Token();
    ~Token();

    explicit Token(const char* str);
    explicit Token(const std::string& str);

    // back() is c++11
    char& back() throw();

    // reading method
    ws::Token& rdword(std::istream& buffer);
    ws::Token& rdline(std::istream& buffer, char delim = '\n');
    ws::Token& rd_http_line(std::istream& buffer);
    ws::Token& rdall(std::istream& buffer);
  };
}
