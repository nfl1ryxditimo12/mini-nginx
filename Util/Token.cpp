#include "Token.hpp"

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

ws::Token &ws::Token::rdword(ws::Buffer &buffer) {
  this->clear();

  if (buffer.eof())
    return *this;


  std::size_t i = buffer.get_offset();
  while (buffer[i] == ' ' || buffer[i] == '\t')
    ++i;


  while (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n' && buffer[i] != '\r') {
    this->push_back(buffer[i]);
    ++i;
  }

  if (!this->length() && (buffer[i] == '\r')) {
    this->push_back(buffer[i]);
  }

  if (((*this)[0] == '\r' || !this->length()) && (buffer[i] == '\n'))
    this->push_back(buffer[i]);

  buffer.advance(i - buffer.get_offset());

  return *this;
}

ws::Token &ws::Token::rdline(ws::Buffer &buffer, char delim) {
  this->clear();

  std::size_t i = buffer.get_offset();
  for (; !buffer.eof(); ++i) {
    if (buffer[i] == delim) {
      ++i;
      break;
    }

    this->push_back(buffer[i]);
  }

  buffer.advance(i - buffer.get_offset());

  return *this;
}

ws::Token &ws::Token::rd_http_line(ws::Buffer &buffer) {
  this->clear();

  std::size_t i = buffer.get_offset();
  while (buffer[i] == ' ')
    ++i;

  buffer.advance(i - buffer.get_offset());

  for (char c, prev = 0; !buffer.eof(); prev = c) {
    c = buffer.get();
    this->push_back(c);

    if ((c == '\n') && (prev == '\r')) {
      break;
    }
  }


  return *this;
}

ws::Token &ws::Token::rdall(ws::Buffer &buffer) {
  this->clear();

  std::size_t i = buffer.get_offset();
  for (; !buffer.eof(); ++i)
    this->push_back(buffer[i]);

  buffer.advance(i - buffer.get_offset());

  return *this;
}

ws::Buffer &ws::operator<<(ws::Buffer &buffer, ws::Token &token) {
  std::size_t offset = buffer.get_offset();

  for (std::string::size_type i = 0; i < token.length(); ++i)
    buffer[offset + i] = token[i];

  buffer.advance(token.length());

  return buffer;
}
