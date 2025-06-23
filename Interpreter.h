#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"
#include "Environment.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <variant>
#include <string>

using Value = std::variant<int, float, char, std::string>;

class Interpreter {
public:
    Interpreter();

    // Interpret a list of statements
    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    Environment env;
    bool breakLoop = false;
    bool continueLoop = false;

    // Evaluate an expression
    Value evaluateExpr(const Expr* expr);

    // Execute a statement
    void executeStmt(const Stmt* stmt);

    // Helpers for binary and unary operations
    Value applyBinaryOperator(const Token& op, const Value& left, const Value& right);
    Value applyUnaryOperator(const Token& op, const Value& operand);

    // Truthiness helper
    bool isTruthy(const Value& value);

    // Utility: print a value
    void printValue(const Value& value);
};

#endif // INTERPRETER_H
