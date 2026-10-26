// ast.hpp

#pragma once
#include <error.h>
#include <memory>
#include <string>
#include <vector>

#include "common/error.hpp"
#include "common/types.hpp"
#include "runtime/execution_context.hpp"
#include "runtime/value.hpp"
#include "common/op.hpp"

class Environment;

class Expr {
public:
    SourceLocation loc;

    Expr(SourceLocation l) : loc(l) {}

    virtual ~Expr() = default;
    virtual Value eval(ExecutionContext& exe_cont) = 0;
    virtual bool isCallable() const {
        return false;
    }
};

class NumberExpr : public Expr {
public:
    int value;
    NumberExpr(int v, SourceLocation l) : Expr(l), value(v) {}

    Value eval(ExecutionContext& exe_cont) override;
};

class FloatExpr : public Expr {
public:
    double value;
    FloatExpr(double v, SourceLocation l) : Expr(l), value(v) {}

    Value eval(ExecutionContext& exe_cont) override;
};

class UnaryOp : public Expr {
public:
    Op op;
    std::unique_ptr<Expr> operand;

    UnaryOp(Op o, std::unique_ptr<Expr> oper, SourceLocation l)
        : Expr(l), op(o), operand(std::move(oper)) {}

    Value eval(ExecutionContext& exe_cont);
};

class StringExpr : public Expr {
public:
    std::string value;
    StringExpr(std::string v, SourceLocation l) : Expr(l), value(v) {}

    Value eval(ExecutionContext& exe_cont) override;
};

class BoolExpr : public Expr {
public:
    bool value;
    BoolExpr(bool v, SourceLocation l) : Expr(l), value(v) {}

    Value eval(ExecutionContext& exe_cont) override;
};

class ListExpr : public Expr {
public:
    std::vector<std::unique_ptr<Expr>> elements;

    ListExpr(std::vector<std::unique_ptr<Expr>> elems, SourceLocation l) : Expr(l), elements(std::move(elems)) {}

    Value eval(ExecutionContext& exe_cont) override;
};

class VariableExpr : public Expr {
public:
    std::string name;

    VariableExpr(const std::string& n, SourceLocation l) : Expr(l), name(n) {}

    Value eval(ExecutionContext& exe_cont) override;
};

class BinaryExpr : public Expr {
public:
    Op op;

    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    Value eval(ExecutionContext& exe_cont) override;

    BinaryExpr(
        Op o,
        std::unique_ptr<Expr> location,
        std::unique_ptr<Expr> r,
        SourceLocation l
    )
        : Expr(l), op(o), left(std::move(location)), right(std::move(r)) {}
};

class CallExpr : public Expr {
public:
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;

    CallExpr(std::string name, std::vector<std::unique_ptr<Expr>> a, SourceLocation l)
        : Expr(l), callee(std::move(name)), args(std::move(a)) {}

    bool isCallable() const override {
        return true;
    }

    Value eval(ExecutionContext& exe_cont) override;
};

class MemberCallExpr : public Expr {
public:
    std::unique_ptr<Expr> object;
    std::string method;
    std::vector<std::unique_ptr<Expr>> args;

    MemberCallExpr(std::unique_ptr<Expr> obj, std::string met, std::vector<std::unique_ptr<Expr>> a, SourceLocation l)
        : Expr(l), object(std::move(obj)), method(std::move(met)), args(std::move(a)) {}

    Value eval(ExecutionContext& exe_cont) override;
};

class ModuleCallExpr : public Expr {
public:
    std::unique_ptr<Expr> object;
    std::string method;
    std::vector<std::unique_ptr<Expr>> args;

    ModuleCallExpr(std::unique_ptr<Expr> obj, std::string met, std::vector<std::unique_ptr<Expr>> a, SourceLocation l)
        : Expr(l), object(std::move(obj)), method(std::move(met)), args(std::move(a)) {}

    Value eval(ExecutionContext& exe_cont) override;
};

class GetExpr : public Expr {
public:
    std::unique_ptr<Expr> object;
    std::string name;

    GetExpr(std::unique_ptr<Expr> obj, std::string& n, SourceLocation l)
        : Expr(l), object(std::move(obj)), name(n) {}

    Value eval(ExecutionContext& exe_cont) override;
};

class IndexExpr : public Expr {
public:
    std::unique_ptr<Expr> object;
    std::unique_ptr<Expr> index;

    IndexExpr(std::unique_ptr<Expr> obj, std::unique_ptr<Expr> idx, SourceLocation l)
        : Expr(l), object(std::move(obj)), index(std::move(idx)) {}

    Value eval(ExecutionContext& exe_cont) override;
};

class Stmt {
public:

    SourceLocation loc;
    Stmt(SourceLocation location) : loc(location) {}

    virtual ~Stmt() = default;
    virtual void execute(ExecutionContext& exe_cont) = 0;
};

class VarStmt : public Stmt {
public:
    std::string name;
    ValueType declaredType;
    std::unique_ptr<Expr> value;

    VarStmt(std::string n, ValueType type, std::unique_ptr<Expr> v, SourceLocation l)
        : Stmt(l), name(std::move(n)), declaredType(type), value(std::move(v)) {}

    void execute(ExecutionContext& exe_cont) override;
};

class ExpressionStmt : public Stmt {
public:
    std::vector<std::unique_ptr<Expr>> values;

    ExpressionStmt(std::vector<std::unique_ptr<Expr>> e, SourceLocation l)
        : Stmt(l), values(std::move(e)) {};

    void execute(ExecutionContext& exe_cont) override;
};

class AssignStmt : public Stmt {
public:
    std::string name;
    std::unique_ptr<Expr> value;

    AssignStmt(std::string n, std::unique_ptr<Expr> v, SourceLocation l)
        : Stmt(l), name(std::move(n)), value(std::move(v)) {}

    void execute(ExecutionContext& exe_cont) override;
};

class DeleteStmt : public Stmt {
public:
    std::vector<std::string> names;

    DeleteStmt(std::vector<std::string> n, SourceLocation l)
        : Stmt(l), names(std::move(n)) {}

    void execute(ExecutionContext& exe_cont) override;
};

class FunStmt : public Stmt {
public:
    std::string name;
    ValueType type;
    std::vector<std::string> args;
    std::vector<std::unique_ptr<Stmt>> body;

    FunStmt(std::string n, ValueType t, std::vector<std::string> arg, std::vector<std::unique_ptr<Stmt>> b, SourceLocation l)
        : Stmt(l), name(std::move(n)), type(std::move(t)), args(std::move(arg)), body(std::move(b)) {}

    void execute(ExecutionContext& exe_cont) override;
};

class IfStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> thenBranch;
    std::vector<std::unique_ptr<Stmt>> elseBranch;

    IfStmt(std::unique_ptr<Expr> cond,
           std::vector<std::unique_ptr<Stmt>> thenB,
           std::vector<std::unique_ptr<Stmt>> elseB, SourceLocation l)
        : Stmt(l),
          condition(std::move(cond)),
          thenBranch(std::move(thenB)),
          elseBranch(std::move(elseB)) {}

    void execute(ExecutionContext& exe_cont) override;
};

class WhileStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> body;

    WhileStmt(std::unique_ptr<Expr> cond, std::vector<std::unique_ptr<Stmt>> b, SourceLocation l)
        : Stmt(l), condition(std::move(cond)), body(std::move(b)) {}

    void execute(ExecutionContext& exe_cont) override;
};

class ForStmt : public Stmt {
public:
    std::string variable;
    std::unique_ptr<Expr> iterable;
    std::vector<std::unique_ptr<Stmt>> body;

    ForStmt(std::string var, std::unique_ptr<Expr> iter, std::vector<std::unique_ptr<Stmt>> b, SourceLocation l)
        : Stmt(l), variable(std::move(var)), iterable(std::move(iter)), body(std::move(b)) {}

    void execute(ExecutionContext& exe_cont) override;
};

class ImportStmt : public Stmt {
public:
    std::vector<std::string> names;

    ImportStmt(std::vector<std::string> n, SourceLocation l)
        : Stmt(l), names(std::move(n)) {}

    void execute(ExecutionContext& exe_cont) override;
};
