#include "Tokenizer.h"
#include <cctype>
#include <unordered_map>

// Keyword lookup table
static std::unordered_map<std::string, TokenType> keywords = {
    {"print",    TokenType::Print},
    {"if",       TokenType::If},
    {"else",     TokenType::Else},
    {"while",    TokenType::While},
    {"for",      TokenType::For},
    {"break",    TokenType::Break},
    {"continue", TokenType::Continue}
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
    bool isFloat = false;

    while (isdigit(peek())) advance();

    if (peek() == '.' && isdigit(peekNext())) {
        isFloat = true;
        advance(); // consume '.'
        while (isdigit(peek())) advance();
    }

    std::string text = source.substr(start, pos - start);
    return makeToken(isFloat ? TokenType::Float : TokenType::Integer, text);
}

Token Tokenizer::stringLiteral() {
    size_t start = pos;
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\n') line++;
        advance();
    }

    if (peek() == '"') advance(); // consume closing quote

    std::string text = source.substr(start, pos - start - 1);
    return makeToken(TokenType::String, text);
}

Token Tokenizer::charLiteral() {
    size_t start = pos;

    if (peek() != '\'' || peekNext() == '\0') return makeToken(TokenType::Unknown, "'");

    char c = advance(); // character
    if (match('\'')) {
        return makeToken(TokenType::Char, std::string(1, c));
    }

    return makeToken(TokenType::Unknown, source.substr(start - 1, 2));
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
        case '"':
            return stringLiteral();
        case '\'':
            return charLiteral();
    }

    return makeToken(TokenType::Unknown, std::string(1, c));
}
