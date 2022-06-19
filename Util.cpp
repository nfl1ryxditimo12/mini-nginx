#include "Util.hpp"

#include <fstream>
#include <stdexcept>
#include <sstream>

#include <cstdlib>

#include <unistd.h>

// hash for checking executed directory
static const std::string hash = "Seunkim123rootjaham456projectyeju789dir";

// current executed directory
// initialized by calling get_curr_dir()
static std::string curr_dir;

// param: void
// return: void
// exception: throws when allocation failed
// description: initialize curr_dir
static void set_curr_dir() {
  char* c_curr_dir = getcwd(NULL, 0);
  curr_dir = c_curr_dir;
  free(c_curr_dir);
}

namespace ws {
  // param: void
  // return: (std::string) current executed directory
  // exception: none
  // description: returns curr_dir
  std::string get_curr_dir() throw() {
    if (!curr_dir.length())
      set_curr_dir();

    return curr_dir;
  }

  // param: void
  // return: void
  // exception: throws when current working directory is not project root
  // description: check executed directory is root
  void check_executed_dir() {
    std::fstream identifier;
    identifier.open(get_curr_dir() + "/project_id", std::fstream::in);

    if (identifier.fail() || identifier.bad())
      throw std::runtime_error("Error: webserv must executed in project root path");

    std::stringstream buffer;
    buffer << identifier.rdbuf();
    identifier.close();

    std::string line;
    std::getline(buffer, line);

    if (line != hash || !buffer.eof())
      throw std::runtime_error("Error: webserv must executed in project root path");
  }

  /* TODO
  // param: line to search, character to skip
  // return: (std::string::size_type) index of first occurence of not chr
  // exception: none
  // description: find first occurence of not chr
  std::string::size_type skip_chr(const std::string& line, char chr) throw() {
    
  }

  // param: line to search, string of characters to skip
  // return: (std::string::size_type) index of first occurence of not chr in string
  // exception: none
  // description: find first occurence of not chr in string
  std::string::size_type skip_chr(const std::string& line, const std::string& chrs) {

  }
  */

  // param: line to search
  // return: (std::string::size_type) index of first occurence of not whitespace(space, tab)
  // exception: none
  // description: find first occurence of not whitespace
  std::string::size_type skip_whitespace(const std::string& line) {
    return line.find_first_not_of(" \t");
  }

  // param: line to search, search start position
  // return: (std::string::size_type) index of first occurence of not whitespace(space, tab) after pos
  // exception: none
  // description: find first occurence of not whitespace after pos
  std::string::size_type skip_whitespace(const std::string& line, std::string::size_type pos) {
    return line.find_first_not_of(" \t", pos);
  }
}
