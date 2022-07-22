#pragma once

#include <limits>
#include <string>

#include <cstddef>

#include <unistd.h>

namespace ws {
  class Buffer {
  private:
    // 1MB
    static const std::size_t _kBufferSize = 1024 * 1024;

    char* _buf;
    std::size_t _offset;
    std::size_t _size;

    Buffer& operator=(const Buffer& other);

  public:
    Buffer();
    Buffer(const Buffer& other);
    ~Buffer();

    char& operator[](std::size_t n);
    char& operator[](std::size_t n) const;
    char* operator+(std::size_t n);

    void init_buf();
    void delete_buf();

    char& get();
    void  put(char c);
    void  clear();

    void advance(std::size_t n = std::numeric_limits<std::size_t>::max());
    void rewind(std::size_t n);

    std::size_t size() const;
    bool eof() const;

    char* data() const throw();
    std::size_t get_offset() const throw();

    // should call with kevent
    ssize_t read_file(int fd);
  };

  std::string& operator<<(std::string& str, ws::Buffer& buffer);
}
