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

    // Keyword lookup table
    const std::unordered_map<std::string, TokenType> keywords = {
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"while", TokenType::While},
        {"for", TokenType::For},
        {"print", TokenType::Print},
        {"break", TokenType::Break},
        {"continue", TokenType::Continue}
    };

    // Helpers
    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);
    void skipWhitespace();

    Token identifier();
    Token number();         // Handles both Integer and Float
    Token stringLiteral();  // For "string"
    Token charLiteral();    // For 'c'
    Token makeToken(TokenType type, const std::string& text);

    TokenType checkKeyword(const std::string& text);
};

#endif // TOKENIZER_H
