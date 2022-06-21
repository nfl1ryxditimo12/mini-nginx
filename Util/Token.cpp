#include "Token.hpp"
#include <iostream> // todo
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
    return _token == "\n" || !_token.length();
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

  std::string& ws::Token::erase(size_type index, size_type count) {
    return _token.erase(index, count);
  }

  std::string ws::Token::substr(size_type pos, size_type count) const {
    return _token.substr(pos, count);
  }

  const char* ws::Token::c_str() const {
    return _token.c_str();
  }

  int ws::Token::compare(const std::string& str) const throw() {
    return _token.compare(str);
  }

  int ws::Token::compare(size_type pos1, size_type count1, const std::string& str) const {
    return _token.compare(pos1, count1, str);
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

    while (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
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

  std::ostream& operator<<(std::ostream& stream, const ws::Token& token) {
    stream << token.get_token();
    return stream;
  }

  bool operator==(const ws::Token& lhs, const ws::Token& rhs) throw() {
    return lhs.get_token() == rhs.get_token();
  }

  bool operator!=(const ws::Token& lhs, const ws::Token& rhs) throw() {
    return !(lhs == rhs);
  }
}
