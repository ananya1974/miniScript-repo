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

Value Interpreter::evaluateExpr(const Expr* expr) {
    if (auto intExpr = dynamic_cast<const IntExpr*>(expr)) {
        return intExpr->value;
    }

    if (auto floatExpr = dynamic_cast<const FloatExpr*>(expr)) {
        return floatExpr->value;
    }

    if (auto charExpr = dynamic_cast<const CharExpr*>(expr)) {
        return charExpr->value;
    }

    if (auto stringExpr = dynamic_cast<const StringExpr*>(expr)) {
        return stringExpr->value;
    }

    if (auto var = dynamic_cast<const VariableExpr*>(expr)) {
        if (!env.exists(var->name))
            throw std::runtime_error("Undefined variable: " + var->name);
        return env.get(var->name);
    }

    if (auto bin = dynamic_cast<const BinaryExpr*>(expr)) {
        Value left = evaluateExpr(bin->left.get());
        Value right = evaluateExpr(bin->right.get());
        return applyBinaryOperator(bin->op, left, right);
    }

    if (auto unary = dynamic_cast<const UnaryExpr*>(expr)) {
        Value operand = evaluateExpr(unary->right.get());
        return applyUnaryOperator(unary->op, operand);
    }

    throw std::runtime_error("Unknown expression type");
}

void Interpreter::executeStmt(const Stmt* stmt) {
    if (breakLoop || continueLoop) return;

    if (auto printStmt = dynamic_cast<const PrintStmt*>(stmt)) {
        Value value = evaluateExpr(printStmt->expression.get());
        printValue(value);
        return;
    }

    if (auto assignStmt = dynamic_cast<const AssignStmt*>(stmt)) {
        Value value = evaluateExpr(assignStmt->value.get());
        env.set(assignStmt->name, value);
        return;
    }

    if (auto ifStmt = dynamic_cast<const IfStmt*>(stmt)) {
        if (isTruthy(evaluateExpr(ifStmt->condition.get()))) {
            executeStmt(ifStmt->thenBranch.get());
        } else if (ifStmt->elseBranch) {
            executeStmt(ifStmt->elseBranch.get());
        }
        return;
    }

    if (auto whileStmt = dynamic_cast<const WhileStmt*>(stmt)) {
        while (isTruthy(evaluateExpr(whileStmt->condition.get()))) {
            breakLoop = false;
            continueLoop = false;
            executeStmt(whileStmt->body.get());
            if (breakLoop) {
                breakLoop = false;
                break;
            }
            if (continueLoop) {
                continueLoop = false;
                continue;
            }
        }
        return;
    }

    if (auto forStmt = dynamic_cast<const ForStmt*>(stmt)) {
        env.pushScope();
        if (forStmt->initializer) executeStmt(forStmt->initializer.get());

        while (!forStmt->condition || isTruthy(evaluateExpr(forStmt->condition.get()))) {
            breakLoop = false;
            continueLoop = false;
            executeStmt(forStmt->body.get());

            if (breakLoop) {
                breakLoop = false;
                break;
            }
            if (continueLoop) {
                continueLoop = false;
                // fall through
            }

            if (forStmt->increment) executeStmt(forStmt->increment.get());
        }

        env.popScope();
        return;
    }

    if (auto blockStmt = dynamic_cast<const BlockStmt*>(stmt)) {
        env.pushScope();
        for (const auto& s : blockStmt->statements) {
            executeStmt(s.get());
            if (breakLoop || continueLoop) break;
        }
        env.popScope();
        return;
    }

    if (dynamic_cast<const BreakStmt*>(stmt)) {
        breakLoop = true;
        return;
    }

    if (dynamic_cast<const ContinueStmt*>(stmt)) {
        continueLoop = true;
        return;
    }

    throw std::runtime_error("Unknown statement type");
}

Value Interpreter::applyBinaryOperator(const Token& op, const Value& left, const Value& right) {
    return std::visit([&](auto l, auto r) -> Value {
        using L = decltype(l);
        using R = decltype(r);

        if constexpr (std::is_arithmetic_v<L> && std::is_arithmetic_v<R>) {
            switch (op.type) {
                case TokenType::Plus: return l + r;
                case TokenType::Minus: return l - r;
                case TokenType::Star: return l * r;
                case TokenType::Slash:
                    if (r == 0) throw std::runtime_error("Division by zero");
                    return l / r;
                case TokenType::DoubleEqual: return l == r;
                case TokenType::NotEqual: return l != r;
                case TokenType::Less: return l < r;
                case TokenType::LessEqual: return l <= r;
                case TokenType::Greater: return l > r;
                case TokenType::GreaterEqual: return l >= r;
                default: break;
            }
        } else if constexpr (std::is_same_v<L, std::string> && std::is_same_v<R, std::string>) {
            if (op.type == TokenType::Plus) return l + r;
            if (op.type == TokenType::DoubleEqual) return l == r;
            if (op.type == TokenType::NotEqual) return l != r;
        } else if constexpr (std::is_same_v<L, char> && std::is_same_v<R, char>) {
            if (op.type == TokenType::DoubleEqual) return l == r;
            if (op.type == TokenType::NotEqual) return l != r;
        }

        throw std::runtime_error("Unsupported binary operation: " + op.text);
    }, left, right);
}

Value Interpreter::applyUnaryOperator(const Token& op, const Value& operand) {
    return std::visit([&](auto val) -> Value {
        using T = decltype(val);
        if constexpr (std::is_arithmetic_v<T>) {
            if (op.type == TokenType::Minus) return -val;
            if (op.type == TokenType::Plus) return val;
            if (op.text == "!") return !val;
        }

        throw std::runtime_error("Unsupported unary operation on type");
    }, operand);
}

bool Interpreter::isTruthy(const Value& value) {
    return std::visit([](auto val) -> bool {
        using T = decltype(val);
        if constexpr (std::is_same_v<T, int>) return val != 0;
        if constexpr (std::is_same_v<T, float>) return val != 0.0f;
        if constexpr (std::is_same_v<T, char>) return val != '\0';
        if constexpr (std::is_same_v<T, std::string>) return !val.empty();
        return true;
    }, value);
}

void Interpreter::printValue(const Value& value) {
    std::visit([](auto val) {
        std::cout << val << std::endl;
    }, value);
}
