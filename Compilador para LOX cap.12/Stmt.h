#pragma once

#include <any>
#include <memory>
#include <utility>
#include <vector>
#include "Token.h"
#include "Expr.h"
using namespace std;

struct Block;
struct Class;
struct Expression;
struct Function;
struct If;
struct Print;
struct Return;
struct Var;
struct While;

struct StmtVisitor {
  virtual any visitBlockStmt(shared_ptr<Block> stmt) = 0;
  virtual any visitClassStmt(shared_ptr<Class> stmt) = 0;
  virtual any visitExpressionStmt(shared_ptr<Expression> stmt) = 0;
  virtual any visitFunctionStmt(shared_ptr<Function> stmt) = 0;
  virtual any visitIfStmt(shared_ptr<If> stmt) = 0;
  virtual any visitPrintStmt(shared_ptr<Print> stmt) = 0;
  virtual any visitReturnStmt(shared_ptr<Return> stmt) = 0;
  virtual any visitVarStmt(shared_ptr<Var> stmt) = 0;
  virtual any visitWhileStmt(shared_ptr<While> stmt) = 0;
  virtual ~StmtVisitor() = default;
};

struct Stmt {
  virtual any accept(StmtVisitor& visitor) = 0;
};

struct Block: Stmt, public enable_shared_from_this<Block> {
  Block(vector<shared_ptr<Stmt>> statements)
    : statements{move(statements)}
  {}

  any accept(StmtVisitor& visitor) override {
    return visitor.visitBlockStmt(shared_from_this());
  }

  const vector<shared_ptr<Stmt>> statements;
};

struct Class: Stmt, public enable_shared_from_this<Class> {
  Class(Token name, shared_ptr<Variable> superclass, vector<shared_ptr<Function>> methods)
    : name{move(name)}, superclass{move(superclass)}, methods{move(methods)}
  {}

  any accept(StmtVisitor& visitor) override {
    return visitor.visitClassStmt(shared_from_this());
  }

  const Token name;
  const shared_ptr<Variable> superclass;
  const vector<shared_ptr<Function>> methods;
};

struct Expression: Stmt, public enable_shared_from_this<Expression> {
  Expression(shared_ptr<Expr> expression)
    : expression{move(expression)}
  {}

  any accept(StmtVisitor& visitor) override {
    return visitor.visitExpressionStmt(shared_from_this());
  }

  const shared_ptr<Expr> expression;
};

struct Function: Stmt, public enable_shared_from_this<Function> {
  Function(Token name, vector<Token> params, vector<shared_ptr<Stmt>> body)
    : name{move(name)}, params{move(params)}, body{move(body)}
  {}

  any accept(StmtVisitor& visitor) override {
    return visitor.visitFunctionStmt(shared_from_this());
  }

  const Token name;
  const vector<Token> params;
  const vector<shared_ptr<Stmt>> body;
};

struct If: Stmt, public enable_shared_from_this<If> {
  If(shared_ptr<Expr> condition, shared_ptr<Stmt> thenBranch, shared_ptr<Stmt> elseBranch)
    : condition{move(condition)}, thenBranch{move(thenBranch)}, elseBranch{move(elseBranch)}
  {}

  any accept(StmtVisitor& visitor) override {
    return visitor.visitIfStmt(shared_from_this());
  }

  const shared_ptr<Expr> condition;
  const shared_ptr<Stmt> thenBranch;
  const shared_ptr<Stmt> elseBranch;
};

struct Print: Stmt, public enable_shared_from_this<Print> {
  Print(shared_ptr<Expr> expression)
    : expression{move(expression)}
  {}

  any accept(StmtVisitor& visitor) override {
    return visitor.visitPrintStmt(shared_from_this());
  }

  const shared_ptr<Expr> expression;
};

struct Return: Stmt, public enable_shared_from_this<Return> {
  Return(Token keyword, shared_ptr<Expr> value)
    : keyword{move(keyword)}, value{move(value)}
  {}

  any accept(StmtVisitor& visitor) override {
    return visitor.visitReturnStmt(shared_from_this());
  }

  const Token keyword;
  const shared_ptr<Expr> value;
};

struct Var: Stmt, public enable_shared_from_this<Var> {
  Var(Token name, shared_ptr<Expr> initializer)
    : name{move(name)}, initializer{move(initializer)}
  {}

  any accept(StmtVisitor& visitor) override {
    return visitor.visitVarStmt(shared_from_this());
  }

  const Token name;
  const shared_ptr<Expr> initializer;
};

struct While: Stmt, public enable_shared_from_this<While> {
  While(shared_ptr<Expr> condition, shared_ptr<Stmt> body)
    : condition{move(condition)}, body{move(body)}
  {}

  any accept(StmtVisitor& visitor) override {
    return visitor.visitWhileStmt(shared_from_this());
  }

  const shared_ptr<Expr> condition;
  const shared_ptr<Stmt> body;
};

