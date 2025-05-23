#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

class Tokenizer {
public:
    Tokenizer(const std::string& source);
    Token getNextToken();

private:
    std::string source;
    size_t pos = 0;
    int line = 1;

    // Helpers
    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);
    void skipWhitespace();

    Token identifier();
    Token number();
    Token makeToken(TokenType type, const std::string& text);

    // Optional: keyword map
    TokenType checkKeyword(const std::string& text);
};

#endif // TOKENIZER_H
