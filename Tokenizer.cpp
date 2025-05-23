#include "Tokenizer.h"
#include <cctype>
#include <unordered_map>

// Keyword lookup table
static std::unordered_map<std::string, TokenType> keywords = {
    {"print", TokenType::Print},
    {"if",    TokenType::If},
    {"else",  TokenType::Else},
    {"while", TokenType::While}
};

Tokenizer::Tokenizer(const std::string& src) : source(src) {}

char Tokenizer::peek() const {
    if (pos >= source.size()) return '\0';
    return source[pos];
}

char Tokenizer::peekNext() const {
    if (pos + 1 >= source.size()) return '\0';
    return source[pos + 1];
}

char Tokenizer::advance() {
    if (pos >= source.size()) return '\0';
    return source[pos++];
}

bool Tokenizer::match(char expected) {
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

void Tokenizer::skipWhitespace() {
    while (true) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '\n') {
            line++;
            advance();
        } else {
            break;
        }
    }
}

Token Tokenizer::makeToken(TokenType type, const std::string& text) {
    return Token(type, text, line);
}

TokenType Tokenizer::checkKeyword(const std::string& text) {
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::Identifier;
}

Token Tokenizer::identifier() {
    size_t start = pos - 1;
    while (isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, pos - start);
    TokenType type = checkKeyword(text);
    return makeToken(type, text);
}

Token Tokenizer::number() {
    size_t start = pos - 1;
    while (isdigit(peek())) advance();
    std::string text = source.substr(start, pos - start);
    return makeToken(TokenType::Number, text);
}

Token Tokenizer::getNextToken() {
    skipWhitespace();

    char c = advance();
    if (c == '\0') return makeToken(TokenType::EndOfFile, "");

    if (isalpha(c) || c == '_') return identifier();

    if (isdigit(c)) return number();

    switch (c) {
        case '+': return makeToken(TokenType::Plus, "+");
        case '-': return makeToken(TokenType::Minus, "-");
        case '*': return makeToken(TokenType::Star, "*");
        case '/': return makeToken(TokenType::Slash, "/");
        case '(': return makeToken(TokenType::LeftParen, "(");
        case ')': return makeToken(TokenType::RightParen, ")");
        case '{': return makeToken(TokenType::LeftBrace, "{");
        case '}': return makeToken(TokenType::RightBrace, "}");
        case ';': return makeToken(TokenType::Semicolon, ";");
        case ',': return makeToken(TokenType::Comma, ",");
        case '=':
            if (match('=')) return makeToken(TokenType::DoubleEqual, "==");
            else return makeToken(TokenType::Equal, "=");
        case '!':
            if (match('=')) return makeToken(TokenType::NotEqual, "!=");
            break;
        case '<':
            if (match('=')) return makeToken(TokenType::LessEqual, "<=");
            else return makeToken(TokenType::Less, "<");
        case '>':
            if (match('=')) return makeToken(TokenType::GreaterEqual, ">=");
            else return makeToken(TokenType::Greater, ">");
    }

    return makeToken(TokenType::Unknown, std::string(1, c));
}
