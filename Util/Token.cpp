#include "Token.hpp"

namespace ws {
  ws::Token::Token() : std::string() {}

  ws::Token::Token(const char* str) : std::string(str) {}

  ws::Token::Token(const std::string& str) : std::string(str) {}

  ws::Token::~Token() {}

  char& ws::Token::back() throw() {
    return this->operator[]((this->length() - 1) * (this->length() != 0));
  }

  /*
  param: input stream to read
  return: *this
  exception: thorws if an error occured while reading buffer
  description: reads buffer ignoring whitespaces
  */
  ws::Token& ws::Token::rdword(std::istream& buffer) {
    this->clear();

    char c = buffer.peek();

    if (buffer.eof())
      return *this;

    while (c == ' ' || c == '\t') {
      buffer.get();
      c = buffer.peek();
    }

    while (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
      this->push_back(buffer.get());
      c = buffer.peek();
    }

    if (!this->length() && (c == '\r')) {
      this->push_back(buffer.get());
      c = buffer.peek();
    }

    if (((*this)[0] == '\r' || !this->length()) && (c == '\n'))
      this->push_back(buffer.get());

    return *this;
  }

  /*
  param: input stream to read, delimeter to end reading
  return: *this
  exception: thorws if an error occured while reading buffer
  description: reads buffer include whitespaces until delim occured
  */
  ws::Token& ws::Token::rdline(std::istream& buffer, char delim) {
    this->clear();

    for (char c = buffer.get(); !buffer.eof(); c = buffer.get()) {
      if (c == delim)
        return *this;

      this->push_back(c);
    }

    return *this;
  }

  /*
  param: input stream to read
  return: *this
  exception: thorws if an error occured while reading buffer
  description: reads buffer includ whitespaces until "\r\n" occured
  */
  ws::Token& ws::Token::rd_http_line(std::istream& buffer) {
    this->clear();

    while (true) {
      if (buffer.peek() != ' ')
        break;

      buffer.get();
    }

    for (char c = buffer.get(), prev = 0; ; prev = c, c = buffer.get()) {
      if (buffer.eof()) {
        return *this;
      }

      this->push_back(c);

      if ((c == '\n') && (prev == '\r'))
          return *this;
    }
  }

  /*
  param: input stream to read
  return: *this
  exception: thorws if an error occured while reading buffer
  description: reads buffer until buffer's eof bit is set or c is NUL
  note: all char in buffer is valid, therefore, should accept all byte
  */
  ws::Token& ws::Token::rdall(std::istream& buffer) {
    this->clear();

    for (char c = buffer.get(); ; c = buffer.get()) {
      if (buffer.eof())
        break;

      this->push_back(c);
    }

    return *this;
  }

  /* TODO
  
  param: void
  return: true if token is "\n"
  exception: none
  description: returns true if Token string is "\n"
  
  bool ws::Token::is_endl() const throw() {
    return _data == "\n" || !_data.length();
  }


  param: void
  return: true if token is "\r\n"
  exception: none
  description: returns true if Token string is "\r\n"

  bool ws::Token::is_http_endl() const throw() {
    return _data == "\r\n";
  }
  */
}
