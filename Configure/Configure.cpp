#include "Configure.hpp"

const ws::ConfParser ws::Configure::parser;

ws::Configure::Configure(const std::string& file, const std::string& curr_dir)
    : server(parser.parse(file, curr_dir)) {}

ws::Configure::~Configure() {}
