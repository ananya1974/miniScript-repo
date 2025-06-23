#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Special tokens
    EndOfFile,
    Unknown,

    // Literals and identifiers
    Identifier,
    Integer,
    Float,
    Char,
    String,

    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Equal,
    DoubleEqual,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,

    // Delimiters
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Semicolon,
    Comma,

    // Keywords
    Print,
    If,
    Else,
    While,
    For,         // <--- NEW
    Break,       // <--- NEW (optional)
    Continue     // <--- NEW (optional)
};

struct Token {
    TokenType type;
    std::string text;
    int line;

    Token(TokenType t, const std::string& txt, int ln)
        : type(t), text(txt), line(ln) {}
};

#endif // TOKEN_H
