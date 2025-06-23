#ifndef AST_H
#define AST_H

#include "Token.h"
#include <memory>
#include <string>
#include <vector>

// --- Forward declarations ---
struct Expr;
struct Stmt;

// --- Expression base ---
struct Expr {
    virtual ~Expr() = default;
};

// --- Expression subclasses ---

struct IntExpr : Expr {
    int value;
    IntExpr(int val) : value(val) {}
};

struct FloatExpr : Expr {
    float value;
    FloatExpr(float val) : value(val) {}
};

struct CharExpr : Expr {
    char value;
    CharExpr(char val) : value(val) {}
};

struct StringExpr : Expr {
    std::string value;
    StringExpr(const std::string& val) : value(val) {}
};

struct VariableExpr : Expr {
    std::string name;
    VariableExpr(const std::string& n) : name(n) {}
};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> l, const Token& oper, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(oper), right(std::move(r)) {}
};

struct UnaryExpr : Expr {
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(const Token& oper, std::unique_ptr<Expr> rhs)
        : op(oper), right(std::move(rhs)) {}
};

// --- Statement base ---
struct Stmt {
    virtual ~Stmt() = default;
};

// --- Statement subclasses ---

struct PrintStmt : Stmt {
    std::unique_ptr<Expr> expression;
    PrintStmt(std::unique_ptr<Expr> expr) : expression(std::move(expr)) {}
};

struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;

    AssignStmt(const std::string& n, std::unique_ptr<Expr> val)
        : name(n), value(std::move(val)) {}
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> thenB, std::unique_ptr<Stmt> elseB)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<Stmt> bod)
        : condition(std::move(cond)), body(std::move(bod)) {}
};

struct ForStmt : Stmt {
    std::unique_ptr<Stmt> initializer;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> increment;
    std::unique_ptr<Stmt> body;

    ForStmt(std::unique_ptr<Stmt> init, std::unique_ptr<Expr> cond,
            std::unique_ptr<Stmt> incr, std::unique_ptr<Stmt> bod)
        : initializer(std::move(init)), condition(std::move(cond)),
          increment(std::move(incr)), body(std::move(bod)) {}
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts) : statements(std::move(stmts)) {}
};

struct BreakStmt : Stmt {};
struct ContinueStmt : Stmt {};

#endif // AST_H
