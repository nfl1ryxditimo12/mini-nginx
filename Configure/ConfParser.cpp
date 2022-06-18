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

std::string  ws::ConfParser::check_block_line(const std::string& line) const {
  std::string::size_type pos = line.find_first_not_of(" \t");
  std::string::size_type keyword_index;

  /* check block option */
  if ((keyword_index = line.find("listen")) != line.npos
      || (keyword_index = line.find("server_names")) != line.npos
      || (keyword_index = line.find("limit_except")) != line.npos
      || (keyword_index = line.find("return")) != line.npos
      || (keyword_index = line.find("client_max_body_size")) != line.npos
      || (keyword_index = line.find("directory_list")) != line.npos
      || (keyword_index = line.find("root")) != line.npos
      || (keyword_index = line.find("index")) != line.npos
      || (keyword_index = line.find("error_page")) != line.npos) {
    if (keyword_index != pos)
      throw std::invalid_argument("Configure: wrong server line");
    if (line.find_first_of(';') + 1 != line.length())
      throw std::invalid_argument("Configure: wrong server line");
  }

  /* check block header */
  else if ((keyword_index = line.find("server")) != line.npos) {
    if (keyword_index != pos)
      throw std::invalid_argument("Configure: wrong server header");
    pos = line.find_first_not_of(" \t", pos + 6); // 6 is "server" length
    if (line.find_first_of('{') != pos)
      throw std::invalid_argument("Configure: wrong server header");
    pos = line.find_first_not_of(" \t", pos + 1); // 1 is "{" length
    if (pos != line.npos)
      throw std::invalid_argument("Configure: wrong server header");
  }
  else if ((keyword_index = line.find("location")) != line.npos) {
    if (keyword_index != pos)
      throw std::invalid_argument("Configure: wrong location header");
    if (line.find_first_of('{') + 1 != line.length())
      throw std::invalid_argument("Configure: wrong location header");
  }

  /* check parenthsis or other */
  else {
    pos = line.find_first_of('}');
    if ((pos == line.npos) || (pos + 1 != line.length()))
      throw std::invalid_argument("Configure: wrong line");
    keyword_index = pos;
  }

  return line.substr(keyword_index, line.find_first_of(" \t", keyword_index) - keyword_index);
}
#include <iostream> // todo
ws::Server ws::ConfParser::parse_server(std::stringstream& buffer, const std::string& root_dir) const throw() {
  ws::Server  server;
  std::string line;
  int         flag = 0;
  (void) root_dir;

  while (!buffer.eof() || !flag) {
    std::getline(buffer, line);
    if (!line.length())
      continue;
    std::string keyword(this->check_block_line(line));
    
    // 각 키워드마다 함수로 빼서 구현을 하면 예외처리도 같이 하면되니까 좋을수도? 
    if (keyword == "listen")
      ;
  }
  return server;
}

std::vector<ws::Server> ws::ConfParser::parse(const std::string& file, const std::string& root_dir) const {
  std::stringstream buffer(this->open_file(file));

  std::string line;
  std::vector<ws::Server> ret;

  std::string curr_path;

  while (!buffer.eof()) {
    ret.push_back(this->parse_server(buffer, root_dir));
  }
  return ret;
}
