#include "ConfParser.hpp"

#include <fstream>
#include <stdexcept>

#include "Util.hpp"

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

void  ws::ConfParser::check_block_header(const std::string& line, const std::string& block_name) const {
  std::string::size_type pos = ws::skip_whitespace(line);

  if (line.find(block_name) != pos)
    throw std::invalid_argument("Configure: wrong " + block_name + " header");

  pos = ws::skip_whitespace(line, pos + 6);
  if (line.find_first_of('{') != pos)
    throw std::invalid_argument("Configure: wrong " + block_name + " header");

  pos = ws::skip_whitespace(line, pos + 1);
  if (pos != line.npos)
    throw std::invalid_argument("Configure: wrong " + block_name + " header");  
}

ws::Server ws::ConfParser::parse_server(std::stringstream& buffer, const std::string& curr_dir) const {
  ws::Server ret;

  std::string line;
  std::string::size_type pos;

  while (!buffer.eof()) {
    std::getline(buffer, line);
    if (!line.length())
      continue;

    pos = ws::skip_whitespace(line);
    if (!line.compare(pos, 6, "listen"))
      ret.set_server_name(this->parse_listen());
  }
}

std::vector<ws::Server> ws::ConfParser::parse(const std::string& file, const std::string& curr_dir) const {
  std::stringstream buffer(this->open_file(file));

  std::string line;
  std::vector<ws::Server> ret;

  while (!buffer.eof()) {
    std::getline(buffer, line);
    if (!line.length())
      continue;

    this->check_block_header(line, "server");
    ret.push_back(this->parse_server(buffer, curr_dir));
  }
  return ret;
}
