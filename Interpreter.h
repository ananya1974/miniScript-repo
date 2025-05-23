#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"
#include "Environment.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>

class Interpreter {
public:
    Interpreter();

    // Interpret a list of statements
    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    Environment env;

    // Evaluate an expression and return its integer value
    int evaluateExpr(const Expr* expr);

    // Execute a statement
    void executeStmt(const Stmt* stmt);

    // Helpers for binary operators
    int applyBinaryOperator(const Token& op, int left, int right);
};

#endif // INTERPRETER_H
