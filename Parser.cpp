#include "Parser.h"
#include <iostream>
#include <stdexcept>

// Simple error reporting helper
static void error(const Token& token, const std::string& message) {
    std::cerr << "[Line " << token.line << "] Error at '" << token.text << "': " << message << std::endl;
    throw std::runtime_error(message);
}

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EndOfFile;
}

const Token& Parser::peek() {
    return tokens[current];
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::match(TokenType type) {
    if (isAtEnd()) return false;
    if (peek().type != type) return false;
    advance();
    return true;
}

const Token& Parser::consume(TokenType type, const std::string& errorMessage) {
    if (peek().type == type) return advance();
    error(peek(), errorMessage);
    throw std::runtime_error("Parse error");
}

// Parsing declarations and statements
std::unique_ptr<Stmt> Parser::declaration() {
    // No separate declarations yet, treat same as statement
    return statement();
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match(TokenType::Print)) return printStatement();
    if (match(TokenType::If)) return ifStatement();
    if (match(TokenType::While)) return whileStatement();
    if (match(TokenType::LeftBrace)) return block();

    if (peek().type == TokenType::Identifier && tokens[current + 1].type == TokenType::Equal)
        return assignment();

    error(peek(), "Expected statement.");
    return nullptr;
}

std::unique_ptr<Stmt> Parser::printStatement() {
    auto exprNode = expression();
    consume(TokenType::Semicolon, "Expect ';' after value.");
    return std::make_unique<PrintStmt>(std::move(exprNode));
}

std::unique_ptr<Stmt> Parser::assignment() {
    Token name = advance(); // consume identifier
    consume(TokenType::Equal, "Expect '=' after variable name.");
    auto value = expression();
    consume(TokenType::Semicolon, "Expect ';' after expression.");
    return std::make_unique<AssignStmt>(name.text, std::move(value));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LeftParen, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RightParen, "Expect ')' after if condition.");

    auto thenBranch = statement();

    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match(TokenType::Else)) {
        elseBranch = statement();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LeftParen, "Expect '(' after 'while'.");
    auto condition = expression();
    consume(TokenType::RightParen, "Expect ')' after while condition.");
    auto body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd() && peek().type != TokenType::RightBrace) {
        statements.push_back(declaration());
    }
    consume(TokenType::RightBrace, "Expect '}' after block.");
    return std::make_unique<BlockStmt>(std::move(statements));
}

// Expression parsing with precedence
std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();

    while (match(TokenType::DoubleEqual) || match(TokenType::NotEqual)) {
        Token op = tokens[current - 1];
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();

    while (match(TokenType::Less) || match(TokenType::LessEqual) || match(TokenType::Greater) || match(TokenType::GreaterEqual)) {
        Token op = tokens[current - 1];
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();

    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        Token op = tokens[current - 1];
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = primary();

    while (match(TokenType::Star) || match(TokenType::Slash)) {
        Token op = tokens[current - 1];
        auto right = primary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if (match(TokenType::Number)) {
        return std::make_unique<NumberExpr>(std::stod(tokens[current - 1].text));
    }

    if (match(TokenType::Identifier)) {
        return std::make_unique<VariableExpr>(tokens[current - 1].text);
    }

    if (match(TokenType::LeftParen)) {
        auto expr = expression();
        consume(TokenType::RightParen, "Expect ')' after expression.");
        return expr;
    }

    error(peek(), "Expected expression.");
    return nullptr;
}
