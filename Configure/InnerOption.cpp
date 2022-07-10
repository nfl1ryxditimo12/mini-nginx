#include "InnerOption.hpp"

#include <stdexcept>

ws::InnerOption::InnerOption() : _autoindex(kAUTOINDEX_UNSET), _client_max_body_size(kCLIENT_MAX_BODY_SIZE_UNSET) {}

ws::InnerOption::InnerOption(const InnerOption& other)
  : _autoindex(other._autoindex),
  _root(other._root),
  _index_set(other._index_set),
  _client_max_body_size(other._client_max_body_size),
  _error_page_map(other._error_page_map) {}

ws::InnerOption& ws::InnerOption::operator=(const InnerOption& other) {
  InnerOption temp(other);

  std::swap(_autoindex, temp._autoindex);
  std::swap(_root, temp._root);
  std::swap(_index_set, temp._index_set);
  std::swap(_client_max_body_size, temp._client_max_body_size);
  std::swap(_error_page_map, temp._error_page_map);

  return *this;
}

ws::InnerOption::~InnerOption() {}

const ws::InnerOption::autoindex_type& ws::InnerOption::get_autoindex() const throw() {
  return _autoindex;
}

const ws::InnerOption::root_type& ws::InnerOption::get_root() const throw() {
  return _root;
}

const ws::InnerOption::index_set_type& ws::InnerOption::get_index_set() const throw() {
  return _index_set;
}

const ws::InnerOption::client_max_body_size_type& ws::InnerOption::get_client_max_body_size() const throw() {
  return _client_max_body_size;
}

const ws::InnerOption::error_page_map_type& ws::InnerOption::get_error_page_map() const throw() {
  return _error_page_map;
}

void ws::InnerOption::set_autoindex(const autoindex_type& value) {
  _autoindex = value;
}

void ws::InnerOption::set_root(const root_type& value) {
  _root = value;
}

void ws::InnerOption::add_index(const index_type& value) {
  _index_set.insert(value);
}

void ws::InnerOption::set_index(const ws::InnerOption::index_set_type &value) {
  _index_set = value;
}

void ws::InnerOption::set_client_max_body_size(const client_max_body_size_type& value) {
  _client_max_body_size = value;
}

void ws::InnerOption::add_error_page(const error_page_type& value) {
  _error_page_map.insert(value);
}
