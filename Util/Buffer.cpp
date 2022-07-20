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

std::string ws::Buffer::rd_line(const std::string& delim) {
  if (!_buf)
    throw std::out_of_range("access after delete");

  while (true) {
    if (_buf[_offset] != ' ')
      break;

    ++_offset;
  }

  std::string ret;
  ret.reserve(1024 * 1024);

  while (
    _offset < _size
    && (
      ret.length() < delim.length()
      || ret.compare(ret.length() - delim.length(), delim.length(), delim))
    ) {
    ret.push_back(_buf[_offset++]);
  }

  return ret;
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

ssize_t ws::Buffer::write_file(int fd, std::size_t n) {
  if (!_buf)
    throw std::out_of_range("access after delete");

  advance(n);
  return write(fd, _buf, n);
}
