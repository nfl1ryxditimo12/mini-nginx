#include "Controller.hpp"

ws::Controller::Controller() {
  _process_map.insert(process_map_type::value_type("GET", &Controller::process_get));
  _process_map.insert(process_map_type::value_type("POST", &Controller::process_get));
  _process_map.insert(process_map_type::value_type("DELETE", &Controller::process_get));
}

ws::Controller::~Controller() {}

bool ws::Controller::is_error_status(unsigned int stat) const throw() {
  return stat != 0;
}

std::string ws::Controller::get_error_response(const ws::Controller::client_value_type &client) {
  // todo
  (void)client;
  return "";
}

std::string ws::Controller::process_get(const ws::Controller::client_value_type &client) {
  ws::Repository* repository = client.repository;
  (void)repository;
  return "";
}

std::string ws::Controller::process_post(const ws::Controller::client_value_type &client) {
  // todo
  (void)client;
  return "";
}

std::string ws::Controller::process_delete(const ws::Controller::client_value_type &client) {
  // todo
  (void)client;
  return "";
}

std::string ws::Controller::get_response(const ws::Controller::client_value_type &client) {
  /* status > 0 인 경우 get_error_response 호출 */
  return (this->*(_process_map.find(client.request->get_method())->second))(client);
}