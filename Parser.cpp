#include "Parser.h"
#include <iostream>
#include <stdexcept>

// --- Error Reporting ---
static void error(const Token& token, const std::string& message) {
    std::cerr << "[Line " << token.line << "] Error at '" << token.text << "': " << message << std::endl;
    throw std::runtime_error(message);
}

// --- Constructor ---
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

// --- Entry Point ---
std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

// --- Helpers ---
bool Parser::isAtEnd() {
    return peek().type == TokenType::EndOfFile;
}

const Token& Parser::peek() {
    return tokens[current];
}

const Token& Parser::previous() {
    return tokens[current - 1];
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::check(TokenType type) {
    return !isAtEnd() && peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    error(peek(), message);
    throw std::runtime_error("Unreachable");  // Prevent warning
}

// --- Declarations and Statements ---
std::unique_ptr<Stmt> Parser::declaration() {
    return statement();
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match(TokenType::Print)) return printStatement();
    if (match(TokenType::If)) return ifStatement();
    if (match(TokenType::While)) return whileStatement();
    if (match(TokenType::For)) return forStatement();
    if (match(TokenType::Break)) return breakStatement();
    if (match(TokenType::Continue)) return continueStatement();
    if (match(TokenType::LeftBrace)) return block();

    if (check(TokenType::Identifier) && current + 1 < tokens.size() && tokens[current + 1].type == TokenType::Equal)
        return assignmentStatement();

    error(peek(), "Expected a statement.");
    return nullptr;
}

std::unique_ptr<Stmt> Parser::printStatement() {
    auto expr = expression();
    consume(TokenType::Semicolon, "Expect ';' after value.");
    return std::make_unique<PrintStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::assignmentStatement() {
    Token name = advance();
    consume(TokenType::Equal, "Expect '=' after variable name.");
    auto value = expression();
    consume(TokenType::Semicolon, "Expect ';' after expression.");
    return std::make_unique<AssignStmt>(name.text, std::move(value));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LeftParen, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RightParen, "Expect ')' after condition.");
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
    consume(TokenType::RightParen, "Expect ')' after condition.");
    auto body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::forStatement() {
    consume(TokenType::LeftParen, "Expect '(' after 'for'.");

    std::unique_ptr<Stmt> initializer;
    if (match(TokenType::Semicolon)) {
        initializer = nullptr;
    } else if (check(TokenType::Identifier) && current + 1 < tokens.size() && tokens[current + 1].type == TokenType::Equal) {
        initializer = assignmentStatement();
    } else {
        error(peek(), "Invalid initializer in 'for' loop.");
    }

    auto condition = expression();
    consume(TokenType::Semicolon, "Expect ';' after loop condition.");

    auto increment = assignmentStatement();
    consume(TokenType::RightParen, "Expect ')' after for clauses.");
    auto body = statement();

    return std::make_unique<ForStmt>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}

std::unique_ptr<Stmt> Parser::breakStatement() {
    consume(TokenType::Semicolon, "Expect ';' after 'break'.");
    return std::make_unique<BreakStmt>();
}

std::unique_ptr<Stmt> Parser::continueStatement() {
    consume(TokenType::Semicolon, "Expect ';' after 'continue'.");
    return std::make_unique<ContinueStmt>();
}

std::unique_ptr<Stmt> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd() && !check(TokenType::RightBrace)) {
        statements.push_back(declaration());
    }
    consume(TokenType::RightBrace, "Expect '}' after block.");
    return std::make_unique<BlockStmt>(std::move(statements));
}

// --- Expression Parsing ---
std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    while (match(TokenType::DoubleEqual) || match(TokenType::NotEqual)) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();
    while (match(TokenType::Less) || match(TokenType::LessEqual) ||
           match(TokenType::Greater) || match(TokenType::GreaterEqual)) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = unary();
    while (match(TokenType::Star) || match(TokenType::Slash)) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match(TokenType::Minus)) {
        Token op = previous();
        auto right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match(TokenType::Integer)) {
        return std::make_unique<IntExpr>(std::stoi(previous().text));
    }
    if (match(TokenType::Float)) {
        return std::make_unique<FloatExpr>(std::stof(previous().text));
    }
    if (match(TokenType::Char)) {
        return std::make_unique<CharExpr>(previous().text[0]);
    }
    if (match(TokenType::String)) {
        return std::make_unique<StringExpr>(previous().text);
    }
    if (match(TokenType::Identifier)) {
        return std::make_unique<VariableExpr>(previous().text);
    }
    if (match(TokenType::LeftParen)) {
        auto expr = expression();
        consume(TokenType::RightParen, "Expect ')' after expression.");
        return expr;
    }

    error(peek(), "Expected expression.");
    return nullptr;
}

