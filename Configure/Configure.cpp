#include "Configure.hpp"

const ws::ConfParser ws::Configure::parser;

ws::Configure::Configure(const std::string& file, const std::string& root_dir)
    : server(parser.parse(file, root_dir)) {}

ws::Configure::~Configure() {}
