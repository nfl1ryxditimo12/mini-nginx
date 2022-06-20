#include "Token.hpp"

namespace ws {
  ws::Token::Token() {}

  ws::Token::~Token() {}

  std::string& ws::Token::get_token() throw() {
    return _token;
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

    while (c != ' ' && c != '\t' && c != '\n') {
      target.push_back(buffer.get());
      c = buffer.peek();
    }

    if (!target.length() && c == '\n')
      target.push_back(buffer.get());

    return token;
  }
}
