#pragma once

#include <any>
#include <memory>
#include <utility>  // move
#include <vector>
#include "Token.h"
using namespace std;


struct Assign;
struct Binary;
struct Call;
struct Get;
struct Grouping;
struct Literal;
struct Logical;
struct Set;
struct Super;
struct This;
struct Unary;
struct Variable;

struct ExprVisitor {
  virtual any visitAssignExpr(shared_ptr<Assign> expr) = 0;
  virtual any visitBinaryExpr(shared_ptr<Binary> expr) = 0;
  virtual any visitCallExpr(shared_ptr<Call> expr) = 0;
  virtual any visitGetExpr(shared_ptr<Get> expr) = 0;
  virtual any visitGroupingExpr(shared_ptr<Grouping> expr) = 0;
  virtual any visitLiteralExpr(shared_ptr<Literal> expr) = 0;
  virtual any visitLogicalExpr(shared_ptr<Logical> expr) = 0;
  virtual any visitSetExpr(shared_ptr<Set> expr) = 0;
  virtual any visitSuperExpr(shared_ptr<Super> expr) = 0;
  virtual any visitThisExpr(shared_ptr<This> expr) = 0;
  virtual any visitUnaryExpr(shared_ptr<Unary> expr) = 0;
  virtual any visitVariableExpr(shared_ptr<Variable> expr) = 0;
  virtual ~ExprVisitor() = default;
};

struct Expr {
  virtual any accept(ExprVisitor& visitor) = 0;
};

struct Assign: Expr, public enable_shared_from_this<Assign> {
  Assign(Token name, shared_ptr<Expr> value)
    : name{move(name)}, value{move(value)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitAssignExpr(shared_from_this());
  }

  const Token name;
  const shared_ptr<Expr> value;
};

struct Binary: Expr, public enable_shared_from_this<Binary> {
  Binary(shared_ptr<Expr> left, Token op, shared_ptr<Expr> right)
    : left{move(left)}, op{move(op)}, right{move(right)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitBinaryExpr(shared_from_this());
  }

  const shared_ptr<Expr> left;
  const Token op;
  const shared_ptr<Expr> right;
};

struct Call: Expr, public enable_shared_from_this<Call> {
  Call(shared_ptr<Expr> callee, Token paren, vector<shared_ptr<Expr>> arguments)
    : callee{move(callee)}, paren{move(paren)}, arguments{move(arguments)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitCallExpr(shared_from_this());
  }

  const shared_ptr<Expr> callee;
  const Token paren;
  const vector<shared_ptr<Expr>> arguments;
};

struct Get: Expr, public enable_shared_from_this<Get> {
  Get(shared_ptr<Expr> object, Token name)
    : object{move(object)}, name{move(name)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitGetExpr(shared_from_this());
  }

  const shared_ptr<Expr> object;
  const Token name;
};

struct Grouping: Expr, public enable_shared_from_this<Grouping> {
  Grouping(shared_ptr<Expr> expression)
    : expression{move(expression)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitGroupingExpr(shared_from_this());
  }

  const shared_ptr<Expr> expression;
};

struct Literal: Expr, public enable_shared_from_this<Literal> {
  Literal(any value)
    : value{move(value)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitLiteralExpr(shared_from_this());
  }

  const any value;
};

struct Logical: Expr, public enable_shared_from_this<Logical> {
  Logical(shared_ptr<Expr> left, Token op, shared_ptr<Expr> right)
    : left{move(left)}, op{move(op)}, right{move(right)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitLogicalExpr(shared_from_this());
  }

  const shared_ptr<Expr> left;
  const Token op;
  const shared_ptr<Expr> right;
};

struct Set: Expr, public enable_shared_from_this<Set> {
  Set(shared_ptr<Expr> object, Token name, shared_ptr<Expr> value)
    : object{move(object)}, name{move(name)}, value{move(value)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitSetExpr(shared_from_this());
  }

  const shared_ptr<Expr> object;
  const Token name;
  const shared_ptr<Expr> value;
};

struct Super: Expr, public enable_shared_from_this<Super> {
  Super(Token keyword, Token method)
    : keyword{move(keyword)}, method{move(method)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitSuperExpr(shared_from_this());
  }

  const Token keyword;
  const Token method;
};

struct This: Expr, public enable_shared_from_this<This> {
  This(Token keyword)
    : keyword{move(keyword)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitThisExpr(shared_from_this());
  }

  const Token keyword;
};

struct Unary: Expr, public enable_shared_from_this<Unary> {
  Unary(Token op, shared_ptr<Expr> right)
    : op{move(op)}, right{move(right)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitUnaryExpr(shared_from_this());
  }

  const Token op;
  const shared_ptr<Expr> right;
};

struct Variable: Expr, public enable_shared_from_this<Variable> {
  Variable(Token name)
    : name{move(name)}
  {}

  any accept(ExprVisitor& visitor) override {
    return visitor.visitVariableExpr(shared_from_this());
  }

  const Token name;
};

