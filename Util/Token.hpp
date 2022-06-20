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
    typedef std::string::size_type  size_type;

    static size_type npos;

    Token();
    Token(char c);
    Token(const char* str);
    Token(const std::string& str);
    ~Token();

    std::string& get_token() throw();
    const std::string& get_token() const throw();

    bool is_endl() const throw();
    bool is_endl_http() const throw();
    size_type find(const std::string& str, size_type pos = 0) const throw();
    size_type length() const throw();

    char& operator[](size_type n) throw();
    const char& operator[](size_type n) const throw();
  };

  ws::Token& operator<<(ws::Token& token, std::istream& buffer);

  bool operator==(const ws::Token& lhs, const ws::Token& rhs) throw();
  bool operator!=(const ws::Token& lhs, const ws::Token& rhs) throw();
}
