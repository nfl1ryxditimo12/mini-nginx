#pragma once

#include <istream>
#include <string>

namespace ws {
  /*
  Token class for parsing
  By calling operator<<, Token extracts string form istream without whitespace
  Any whitespace(` ', `\t') in buffer is ignored
  */
  class Token {
  private:
    std::string _token;

    Token& operator=(const Token& other);
    Token(const Token& other);

  public:
    typedef std::string::size_type  size_type;

    static size_type npos;

    Token();
    ~Token();

    // copy construct for compare
    Token(char c);
    Token(const char* str);
    Token(const std::string& str);

    // getter
    std::string& get_token() throw();
    const std::string& get_token() const throw();

    // custom method
    bool is_endl() const throw();
    bool is_endl_http() const throw();
    int strncmp(const std::string& str, size_type start = 0, size_type len = npos) const throw();

    // std::string method
    size_type find(const std::string& str, size_type pos = 0) const throw();
    size_type length() const throw();
    std::string& erase(size_type index = 0, size_type count = npos);
    std::string substr(size_type pos = 0, size_type count = npos) const;
    const char* c_str() const;
    int compare(const std::string& str) const throw();
    int compare(size_type pos1, size_type count1, const std::string& str) const;

    char& operator[](size_type n) throw();
    const char& operator[](size_type n) const throw();
  };

  ws::Token& operator<<(ws::Token& token, std::istream& buffer);
  std::ostream& operator<<(std::ostream& stream, const ws::Token& token);

  bool operator==(const ws::Token& lhs, const ws::Token& rhs) throw();
  bool operator!=(const ws::Token& lhs, const ws::Token& rhs) throw();
}
