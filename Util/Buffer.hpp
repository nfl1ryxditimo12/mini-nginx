#pragma once

#include <limits>
#include <string>

#include <cstddef>

#include <unistd.h>

namespace ws {
  class Buffer {
  private:
    // 1MB
    static const std::size_t _kBufferSize = 1024 * 1024 * 1024;

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
    char& get();
    char* operator+(std::size_t n);

    void advance(std::size_t n = std::numeric_limits<std::size_t>::max());
    void rewind(std::size_t n);

    void init_buf();
    void delete_buf();

    std::string rd_line(const std::string& delim = "\n");

    void clear();
    std::size_t size() const;
    bool eof() const;

    std::size_t get_offset() const throw();

    // should call with kevent
    ssize_t read_file(int fd);
    ssize_t write_file(int fd, std::size_t n);
  };
}
