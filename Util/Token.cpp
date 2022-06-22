#include "Token.hpp"

namespace ws {
  Token::size_type ws::Token::npos = std::string::npos;

  ws::Token::Token() {}

  ws::Token::Token(char c) {
    _data.push_back(c);
  }

  ws::Token::Token(const char* str) : _data(str) {}

  ws::Token::Token(const std::string& str) : _data(str) {}

  ws::Token::~Token() {}

  std::string& ws::Token::get_data() throw() {
    return _data;
  }

  const std::string& ws::Token::get_data() const throw() {
    return _data;
  }

  /*
  param: input stream to read
  return: *this
  exception: thorws if an error occured while reading buffer
  description: reads buffer ignoring whitespaces
  */
  ws::Token& ws::Token::rdword(std::istream& buffer) {
    _data.clear();

    char c = buffer.peek();

    if (buffer.eof())
      return *this;

    while (c == ' ' || c == '\t') {
      buffer.get();
      c = buffer.peek();
    }

    while (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
      _data.push_back(buffer.get());
      c = buffer.peek();
    }

    if (!_data.length() && (c == '\r')) {
      _data.push_back(buffer.get());
      c = buffer.peek();
    }

    if ((_data[0] == '\r' || !_data.length()) && (c == '\n'))
      _data.push_back(buffer.get());

    return *this;
  }

  /*
  param: input stream to read, delimeter to end reading
  return: *this
  exception: thorws if an error occured while reading buffer
  description: reads buffer include whitespaces until delim occured
  */
  ws::Token& ws::Token::rdline(std::istream& buffer, char delim) {
    _data.clear();

    for (char c = buffer.get(); !buffer.eof(); c = buffer.get()) {
      if (c == delim)
        return *this;

      _data.push_back(c);
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
    _data.clear();

    for (char c = buffer.get(), prev = 0; ; prev = c, c = buffer.get()) {
      if (buffer.eof())
        return *this;

      _data.push_back(c);

      if ((c == '\n') && (prev == '\r')) {
          _data.erase(_data.length() - 2, 2);
          return *this;
      }
    }
  }

  /*
  param: input stream to read
  return: *this
  exception: thorws if an error occured while reading buffer
  description: reads buffer until buffer's eof bit is set or c is NUL
  */
  ws::Token& ws::Token::rdall(std::istream& buffer) {
    _data.clear();

    for (char c = buffer.get(); !buffer.eof() && c; c = buffer.get())
      _data.push_back(c);

    return *this;
  }

  /*
  param: void
  return: true if token is "\n"
  exception: none
  description: returns true if Token string is "\n"
  */
  bool ws::Token::is_endl() const throw() {
    return _data == "\n" || !_data.length();
  }

  /*
  param: void
  return: true if token is "\r\n"
  exception: none
  description: returns true if Token string is "\r\n"
  */
  bool ws::Token::is_http_endl() const throw() {
    return _data == "\r\n";
  }



  ws::Token::size_type ws::Token::find(const std::string& str, size_type pos) const throw() {
    return _data.find(str, pos);
  }

  ws::Token::size_type ws::Token::length() const throw() {
    return _data.length();
  }

  std::string& ws::Token::erase(size_type index, size_type count) {
    return _data.erase(index, count);
  }

  std::string ws::Token::substr(size_type pos, size_type count) const {
    return _data.substr(pos, count);
  }

  const char* ws::Token::c_str() const {
    return _data.c_str();
  }

  int ws::Token::compare(const std::string& str) const throw() {
    return _data.compare(str);
  }

  int ws::Token::compare(size_type pos1, size_type count1, const std::string& str) const {
    return _data.compare(pos1, count1, str);
  }



  char& Token::operator[](size_type n) throw() {
    return _data[n];
  }

  const char& Token::operator[](size_type n) const throw() {
    return _data[n];
  }

  std::ostream& operator<<(std::ostream& stream, const ws::Token& token) {
    stream << token.get_data();
    return stream;
  }

  bool operator==(const ws::Token& lhs, const ws::Token& rhs) throw() {
    return lhs.get_data() == rhs.get_data();
  }

  bool operator!=(const ws::Token& lhs, const ws::Token& rhs) throw() {
    return !(lhs == rhs);
  }
}
