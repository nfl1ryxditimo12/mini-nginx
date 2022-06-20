#include <istream>
#include <string>

namespace ws {
  /*
  Token class for parsing
  By calling operator<<, Token extracts string form istream without whitespace
  Any whitespace(` ', `\t') in buffer is ignored
  */
  class Token {
  private:
    std::string _token;

    Token& operator=(const Token& other);
    Token(const Token& other);

  public:
    Token();
    ~Token();

    std::string& get_token() throw();
  };

  ws::Token& operator<<(ws::Token& token, std::istream& buffer);
}
