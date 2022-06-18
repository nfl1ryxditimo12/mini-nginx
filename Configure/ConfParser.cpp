#include "ConfParser.hpp"

#include <fstream>
#include <stdexcept>

ws::ConfParser::ConfParser() {}

ws::ConfParser::~ConfParser() {}

std::string ws::ConfParser::open_file(const std::string& file) const {
  std::ifstream input;
  if (file.rfind(".conf") != file.length() - 5)
    throw std::invalid_argument("Configure: wrong file extension");

  input.open(file, std::ifstream::in);
  if (input.fail() || input.bad())
    throw std::invalid_argument("Configure: fail to open file");

  std::stringstream buffer;
  buffer << input.rdbuf();
  input.close();

  std::string ret(buffer.str());
  return ret;
}

void  ws::ConfParser::check_block_header(const std::string& line) const {
  std::string::size_type pos = line.find_first_not_of(" \t");

  if (line.find("server") != pos)
    throw std::invalid_argument("Configure: wrong server header");
  pos = line.find_first_not_of(" \t", pos + 6);
  if (line.find_first_of('{') != pos)
    throw std::invalid_argument("Configure: wrong server header");
  pos = line.find_first_not_of(" \t", pos + 1);
  if (pos != line.npos)
    throw std::invalid_argument("Configure: wrong server header");
}

std::vector<ws::Server> ws::ConfParser::parse(const std::string& file, const std::string& root_dir) const {
  std::stringstream buffer(this->open_file(file));

  std::string line;
  std::vector<ws::Server> ret;

  std::string curr_path;

  while (!buffer.eof()) {
    std::getline(buffer, line);
    if (!line.length())
      continue;
    this->check_block_header(line);
    break;
    // ret.push_back(this->parse_server(buffer));
  }
  return ret;
}
