#include "Interpreter.h"

Interpreter::Interpreter() : env() {}

void Interpreter::interpret(const std::vector<std::unique_ptr<Stmt>>& statements) {
    try {
        for (const auto& stmt : statements) {
            executeStmt(stmt.get());
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
}

int Interpreter::evaluateExpr(const Expr* expr) {
    if (auto num = dynamic_cast<const NumberExpr*>(expr)) {
        return static_cast<int>(num->value);
    }

    if (auto var = dynamic_cast<const VariableExpr*>(expr)) {
        if (!env.exists(var->name))
            throw std::runtime_error("Undefined variable: " + var->name);
        return env.get(var->name);
    }

    if (auto bin = dynamic_cast<const BinaryExpr*>(expr)) {
        int left = evaluateExpr(bin->left.get());
        int right = evaluateExpr(bin->right.get());
        return applyBinaryOperator(bin->op, left, right);
    }

    throw std::runtime_error("Unknown expression type");
}

void Interpreter::executeStmt(const Stmt* stmt) {
    if (auto printStmt = dynamic_cast<const PrintStmt*>(stmt)) {
        int value = evaluateExpr(printStmt->expression.get());
        std::cout << value << std::endl;
        return;
    }

    if (auto assignStmt = dynamic_cast<const AssignStmt*>(stmt)) {
        int value = evaluateExpr(assignStmt->value.get());
        env.set(assignStmt->name, value);
        return;
    }

    if (auto ifStmt = dynamic_cast<const IfStmt*>(stmt)) {
        int cond = evaluateExpr(ifStmt->condition.get());
        if (cond) {
            executeStmt(ifStmt->thenBranch.get());
        } else if (ifStmt->elseBranch) {
            executeStmt(ifStmt->elseBranch.get());
        }
        return;
    }

    if (auto whileStmt = dynamic_cast<const WhileStmt*>(stmt)) {
        while (evaluateExpr(whileStmt->condition.get())) {
            executeStmt(whileStmt->body.get());
        }
        return;
    }

    if (auto blockStmt = dynamic_cast<const BlockStmt*>(stmt)) {
        env.pushScope();
        for (const auto& s : blockStmt->statements) {
            executeStmt(s.get());
        }
        env.popScope();
        return;
    }

    throw std::runtime_error("Unknown statement type");
}

int Interpreter::applyBinaryOperator(const Token& op, int left, int right) {
    switch (op.type) {
        case TokenType::Plus: return left + right;
        case TokenType::Minus: return left - right;
        case TokenType::Star: return left * right;
        case TokenType::Slash:
            if (right == 0) throw std::runtime_error("Division by zero");
            return left / right;
        case TokenType::DoubleEqual: return left == right;
        case TokenType::NotEqual: return left != right;
        case TokenType::Less: return left < right;
        case TokenType::LessEqual: return left <= right;
        case TokenType::Greater: return left > right;
        case TokenType::GreaterEqual: return left >= right;
        default:
            throw std::runtime_error("Unknown binary operator: " + op.text);
    }
}
