#include "Token.hpp"

namespace ws {
  ws::Token::Token() {}

  ws::Token::~Token() {}

  std::string& ws::Token::get_token() throw() {
    return _token;
  }

  /*
  param: void
  return: true if token is "\n"
  exception: none
  description: returns true if Token string is "\n"
  */
  int ws::Token::is_endl() const throw() {
    return _token == "\n";
  }

  /*
  param: void
  return: true if token is "\r\n"
  exception: none
  description: returns true if Token string is "\r\n"
  */
  int ws::Token::is_endl_http() const throw() {
    return _token == "\r\n";
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
}
