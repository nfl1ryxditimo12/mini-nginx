#include "Response.hpp"

ws::Response::Response(const client_value_type* const client_data)
  : _request(*(client_data->request)),
  _repo(*(client_data->repository)),
  _status(client_data->status),
  _kernel() {}

ws::Response::~Response() {}

bool ws::Response::is_error_status(unsigned int stat) const throw() {
  return stat != 0;
}

void ws::Response::get_error_response() {
  const error_page_map_type& error_page_map = _repo.get_error_page_map();
  error_page_map_type::const_iterator curr_error = error_page_map.find(_status);

  std::stringstream buffer;

  if (curr_error == error_page_map.end()) {
    this->generate_response_header();
    buffer << _status;
    _response += buffer.str() + " Error";
  }
}

std::string ws::Response::process_get(ws::Response::client_value_type &client) {
  // todo
}

std::string ws::Response::process_post(ws::Response::client_value_type &client) {
  // todo
}

std::string ws::Response::process_delete(ws::Response::client_value_type &client) {

}

void ws::Response::generate_response_header() {
  
}

void ws::Response::read_file(const char* file) {

}

void ws::Response::generate_response() {
  if (_status != 0)
    this->get_error_response();
}
