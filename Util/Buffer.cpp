#include "Buffer.hpp"

#include <algorithm>
#include <stdexcept>

#include <string.h>

ws::Buffer::Buffer() : _buf(NULL), _offset(0), _size(0) {}

ws::Buffer::Buffer(const ws::Buffer &other)
  : _buf(NULL), _offset(other._offset), _size(other._size) {}

ws::Buffer::~Buffer() {
  delete[] _buf;
}

char& ws::Buffer::operator[](std::size_t n) {
  if (!_buf)
    throw std::out_of_range("access after delete");

  return _buf[n];
}

char& ws::Buffer::operator[](std::size_t n) const {
  if (!_buf)
    throw std::out_of_range("access after delete");

  return _buf[n];
}

char& ws::Buffer::get() {
  if (!_buf)
    throw std::out_of_range("access after delete");

  ++_offset;
  return _buf[_offset - 1];
}

void ws::Buffer::put(char c) {
  if (!_buf)
    throw std::out_of_range("access after delete");

  _buf[_size++] = c;
}

char* ws::Buffer::operator+(std::size_t n) {
  if (!_buf)
    throw std::out_of_range("access after delete");

  return _buf + n;
}

void ws::Buffer::advance(std::size_t n) {
  if (!_buf)
    throw std::out_of_range("access after delete");

  _offset += std::min(n, _size - _offset);
}

void ws::Buffer::rewind(std::size_t n) {
  if (!_buf)
    throw std::out_of_range("access after delete");

  _offset -= std::min(n, _offset);
}

void ws::Buffer::clear() {
  if (!_buf)
    throw std::out_of_range("access after delete");

  _offset = 0;
  _size = 0;
}

std::size_t ws::Buffer::size() const {
  if (!_buf)
    throw std::out_of_range("access after delete");

  return _size;
}

bool ws::Buffer::eof() const {
  return _offset == _size;
}

void ws::Buffer::init_buf() {
  if (!_buf)
    _buf = new char[_kBufferSize];
}

void ws::Buffer::delete_buf() {
  delete[] _buf;
  _buf = NULL;
}

char *ws::Buffer::data() const throw() {
  return _buf;
}

std::size_t ws::Buffer::get_offset() const throw() {
  return _offset;
}

// should call with kevent
ssize_t ws::Buffer::read_file(int fd) {
  if (!_buf)
    throw std::out_of_range("access after delete");

  ssize_t ret = read(fd, _buf + _size, _kBufferSize - _size);

  if (ret < 0)
    _size = -1;
  else
    _size += ret;

  return ret;
}

std::string& ws::operator<<(std::string& str, ws::Buffer& buffer) {
  std::size_t offset = buffer.get_offset();

  for (; offset < buffer.size(); ++offset) {
    str.push_back(buffer[offset]);
  }

  buffer.advance();

  return str;
}
