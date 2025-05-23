#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Special tokens
    EndOfFile,
    Unknown,

    // Literals and identifiers
    Identifier,     // Variable names, function names
    Number,         // Integer literals

    // Operators
    Plus,           // +
    Minus,          // -
    Star,           // *
    Slash,          // /
    Equal,          // =
    DoubleEqual,    // ==
    NotEqual,       // !=
    Less,           // <
    LessEqual,      // <=
    Greater,        // >
    GreaterEqual,   // >=

    // Delimiters
    LeftParen,      // (
    RightParen,     // )
    LeftBrace,      // {
    RightBrace,     // }
    Semicolon,      // ;
    Comma,          // ,

    // Keywords
    Print,          // print
    If,             // if
    Else,           // else
    While           // while
};

struct Token {
    TokenType type;
    std::string text;
    int line;

    Token(TokenType t, const std::string& txt, int ln)
        : type(t), text(txt), line(ln) {}
};

#endif // TOKEN_H
