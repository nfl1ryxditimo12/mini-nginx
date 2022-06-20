#include "Token.hpp"

namespace ws {
  Token::size_type ws::Token::npos = std::string::npos;

  ws::Token::Token() {}

  ws::Token::Token(char c) {
    _token.push_back(c);
  }

  ws::Token::Token(const char* str) : _token(str) {}

  ws::Token::Token(const std::string& str) : _token(str) {}

  ws::Token::~Token() {}

  std::string& ws::Token::get_token() throw() {
    return _token;
  }

  const std::string& ws::Token::get_token() const throw() {
    return _token;
  }

  /*
  param: void
  return: true if token is "\n"
  exception: none
  description: returns true if Token string is "\n"
  */
  bool ws::Token::is_endl() const throw() {
    return _token == "\n";
  }

  /*
  param: void
  return: true if token is "\r\n"
  exception: none
  description: returns true if Token string is "\r\n"
  */
  bool ws::Token::is_endl_http() const throw() {
    return _token == "\r\n";
  }

  ws::Token::size_type ws::Token::find(const std::string& str, size_type pos) const throw() {
    return _token.find(str, pos);
  }

  ws::Token::size_type ws::Token::length() const throw() {
    return _token.length();
  }

  char& Token::operator[](size_type n) throw() {
    return _token[n];
  }

  const char& Token::operator[](size_type n) const throw() {
    return _token[n];
  }

  /*
  param: Token to store, istream to read
  return: returns Token&
  exception: throws if error in extracting istream or push_back to Token
  description: extract std::string form buffer without whitespace
  */
  ws::Token& operator<<(ws::Token& token, std::istream& buffer) {
    std::string& target = token.get_token();

    target.clear();

    char c = buffer.peek();

    if (buffer.eof())
      return token;

    while (c == ' ' || c == '\t') {
      buffer.get();
      c = buffer.peek();
    }

    while (c != ' ' && c != '\t' && c != '\n' && '\r') {
      target.push_back(buffer.get());
      c = buffer.peek();
    }

    if (!target.length() && (c == '\r')) {
      target.push_back(buffer.get());
      c = buffer.peek();
    }

    if ((target[0] == '\r' || !target.length()) && (c == '\n'))
      target.push_back(buffer.get());

    return token;
  }

  bool operator==(const ws::Token& lhs, const ws::Token& rhs) throw() {
    return lhs.get_token() == rhs.get_token();
  }

  bool operator!=(const ws::Token& lhs, const ws::Token& rhs) throw() {
    return !(lhs == rhs);
  }
}
