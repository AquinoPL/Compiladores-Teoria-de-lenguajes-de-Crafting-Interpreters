#pragma once

#include <any>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>      
#include "Error.h"
#include "Expr.h"
#include "LoxCallable.h"
#include "LoxClass.h"
#include "LoxFunction.h"
#include "LoxInstance.h"
#include "Stmt.h"
using namespace std;

struct LoxReturn {
  const any value;
};

class Environment: public enable_shared_from_this<Environment> {
  friend class Interpreter;

  shared_ptr<Environment> enclosing;
  map<string, any> values;

public:
  Environment()
    : enclosing{nullptr}
  {}

  Environment(shared_ptr<Environment> enclosing)
    : enclosing{move(enclosing)}
  {}

  any get(const Token& name) {
    auto elem = values.find(name.lexeme);
    if (elem != values.end()) {
      return elem->second;
    }

    if (enclosing != nullptr) return enclosing->get(name);

    throw RuntimeError(name,
        "Undefined variable '" + name.lexeme + "'.");
  }

  void assign(const Token& name, any value) {
    auto elem = values.find(name.lexeme);
    if (elem != values.end()) {
      elem->second = move(value);
      return;
    }

    if (enclosing != nullptr) {
      enclosing->assign(name, move(value));
      return;
    }

    throw RuntimeError(name,
        "Undefined variable '" + name.lexeme + "'.");
  }

  void define(const string& name, any value) {
    values[name] = move(value);
  }

  shared_ptr<Environment> ancestor(int distance) {
    shared_ptr<Environment> environment = shared_from_this();
    for (int i = 0; i < distance; ++i) {
      environment = environment->enclosing;
    }

    return environment;
  }

  any getAt(int distance, const string& name) {
    return ancestor(distance)->values[name];
  }

  void assignAt(int distance, const Token& name, any value) {
    ancestor(distance)->values[name.lexeme] = move(value);
  }
};

class NativeClock: public LoxCallable {
public:
  int arity() override { return 0; }

  any call(Interpreter& interpreter,
                vector<any> arguments) override {
    auto ticks = chrono::system_clock::now().time_since_epoch();
    return chrono::duration<double>{ticks}.count() / 1000.0;
  }

  string toString() override { return "<native fn>"; }
};

class Interpreter: public ExprVisitor,
                   public StmtVisitor {
friend class LoxFunction;

public:  shared_ptr<Environment> globals{new Environment};
private:
  shared_ptr<Environment> environment = globals;
  map<shared_ptr<Expr>, int> locals;

public:
  Interpreter() {
    globals->define("clock", shared_ptr<NativeClock>{});
  }

  void interpret(const vector<
      shared_ptr<Stmt>>& statements) {
    try {
      for (const shared_ptr<Stmt>& statement : statements) {
        execute(statement);
      }
    } catch (RuntimeError error) {
      runtimeError(error);
    }
  }

private:
  any evaluate(shared_ptr<Expr> expr) {
    return expr->accept(*this);
  }

  void execute(shared_ptr<Stmt> stmt) {
    stmt->accept(*this);
  }

public:
  void resolve(shared_ptr<Expr> expr, int depth) {
    locals[expr] = depth;
  }

private:
  void executeBlock(
      const vector<shared_ptr<Stmt>>& statements,
      shared_ptr<Environment> environment) {
    shared_ptr<Environment> previous = this->environment;
    try {
      this->environment = environment;

      for (const shared_ptr<Stmt>& statement : statements) {
        execute(statement);
      }
    } catch (...) {
      this->environment = previous;
      throw;
    }

    this->environment = previous;
  }

public:
  any visitBlockStmt(shared_ptr<Block> stmt) override {
    executeBlock(stmt->statements,
                 make_shared<Environment>(environment));
    return {};
  }

  any visitClassStmt(shared_ptr<Class> stmt) override {
    any superclass;
    if (stmt->superclass != nullptr) {
      superclass = evaluate(stmt->superclass);
      if (superclass.type() != typeid(shared_ptr<LoxClass>)) {
        throw RuntimeError(stmt->superclass->name,
            "Superclass must be a class.");
      }
    }

    environment->define(stmt->name.lexeme, nullptr);

    if (stmt->superclass != nullptr) {
      environment = make_shared<Environment>(environment);
      environment->define("super", superclass);
    }

    map<string, shared_ptr<LoxFunction>> methods;
    for (shared_ptr<Function> method : stmt->methods) {
      auto function = make_shared<LoxFunction>(method,
          environment, method->name.lexeme == "init");
      methods[method->name.lexeme] = function;
    }

    shared_ptr<LoxClass> superklass = nullptr;
    if (superclass.type() == typeid(shared_ptr<LoxClass>)) {
      superklass = any_cast<
          shared_ptr<LoxClass>>(superclass);
    }
    auto klass = make_shared<LoxClass>(stmt->name.lexeme,
        superklass, methods);

    if (superklass != nullptr) {
      environment = environment->enclosing;
    }

    environment->assign(stmt->name, move(klass));
    return {};
  }

  any visitExpressionStmt(
      shared_ptr<Expression> stmt) override {
    evaluate(stmt->expression);
    return {};
  }

  any visitFunctionStmt(
      shared_ptr<Function> stmt) override {
    auto function = make_shared<LoxFunction>(stmt, environment,
                                                  false);
    environment->define(stmt->name.lexeme, function);
    return {};
  }

  any visitIfStmt(shared_ptr<If> stmt) override {
    if (isTruthy(evaluate(stmt->condition))) {
      execute(stmt->thenBranch);
    } else if (stmt->elseBranch != nullptr) {
      execute(stmt->elseBranch);
    }
    return {};
  }

  any visitPrintStmt(shared_ptr<Print> stmt) override {
    any value = evaluate(stmt->expression);
    cout << stringify(value) << "\n";
    return {};
  }

  any visitReturnStmt(shared_ptr<Return> stmt) override {
    any value = nullptr;
    if (stmt->value != nullptr) value = evaluate(stmt->value);

    throw LoxReturn{value};
  }

  any visitVarStmt(shared_ptr<Var> stmt) override {
    any value = nullptr;
    if (stmt->initializer != nullptr) {
      value = evaluate(stmt->initializer);
    }

    environment->define(stmt->name.lexeme, move(value));
    return {};
  }

  any visitWhileStmt(shared_ptr<While> stmt) override {
    while (isTruthy(evaluate(stmt->condition))) {
      execute(stmt->body);
    }
    return {};
  }

  any visitAssignExpr(shared_ptr<Assign> expr) override {
    any value = evaluate(expr->value);

    auto elem = locals.find(expr);
    if (elem != locals.end()) {
      int distance = elem->second;
      environment->assignAt(distance, expr->name, value);
    } else {
      globals->assign(expr->name, value);
    }

    return value;
  }

  any visitBinaryExpr(shared_ptr<Binary> expr) override {
    any left = evaluate(expr->left);
    any right = evaluate(expr->right);

    switch (expr->op.type) {
      case BANG_EQUAL: return !isEqual(left, right);
      case EQUAL_EQUAL: return isEqual(left, right);
      case GREATER:
        checkNumberOperands(expr->op, left, right);
        return any_cast<double>(left) >
               any_cast<double>(right);
      case GREATER_EQUAL:
        checkNumberOperands(expr->op, left, right);
        return any_cast<double>(left) >=
               any_cast<double>(right);
      case LESS:
        checkNumberOperands(expr->op, left, right);
        return any_cast<double>(left) <
               any_cast<double>(right);
      case LESS_EQUAL:
        checkNumberOperands(expr->op, left, right);
        return any_cast<double>(left) <=
               any_cast<double>(right);
      case MINUS:
        checkNumberOperands(expr->op, left, right);
        return any_cast<double>(left) -
               any_cast<double>(right);
      case PLUS:
        if (left.type() == typeid(double) &&
            right.type() == typeid(double)) {
          return any_cast<double>(left) +
                 any_cast<double>(right);
        }

        if (left.type() == typeid(string) &&
            right.type() == typeid(string)) {
          return any_cast<string>(left) +
                 any_cast<string>(right);
        }

        throw RuntimeError{expr->op,
            "Operands must be two numbers or two strings."};
      case SLASH:
        checkNumberOperands(expr->op, left, right);
        return any_cast<double>(left) /
               any_cast<double>(right);
      case STAR:
        checkNumberOperands(expr->op, left, right);
        return any_cast<double>(left) *
               any_cast<double>(right);
    }
    
    return {};
  }

  any visitCallExpr(shared_ptr<Call> expr) override {
    any callee = evaluate(expr->callee);

    vector<any> arguments;
    for (const shared_ptr<Expr>& argument : expr->arguments) {
      arguments.push_back(evaluate(argument));
    }

    shared_ptr<LoxCallable> function;

    if (callee.type() == typeid(shared_ptr<LoxFunction>)) {
      function = any_cast<shared_ptr<LoxFunction>>(callee);
    } else if (callee.type() == typeid(shared_ptr<LoxClass>)) {
      function = any_cast<shared_ptr<LoxClass>>(callee);
    } else if (callee.type() ==
         typeid(shared_ptr<NativeClock>)) {
       function = any_cast<shared_ptr<NativeClock>>(callee);
    } else {
      throw RuntimeError{expr->paren,
          "Can only call functions and classes."};
    }

    if (arguments.size() != function->arity()) {
      throw RuntimeError{expr->paren, "Expected " +
          to_string(function->arity()) + " arguments but got " +
          to_string(arguments.size()) + "."};
    }

    return function->call(*this, move(arguments));
  }

  any visitGetExpr(shared_ptr<Get> expr) override {
    any object = evaluate(expr->object);
    if (object.type() == typeid(shared_ptr<LoxInstance>)) {
      return any_cast<
          shared_ptr<LoxInstance>>(object)->get(expr->name);
    }

    throw RuntimeError(expr->name,
        "Only instances have properties.");
  }

  any visitGroupingExpr(
      shared_ptr<Grouping> expr) override {
    return evaluate(expr->expression);
  }

  any visitLiteralExpr(shared_ptr<Literal> expr) override {
    return expr->value;
  }

  any visitLogicalExpr(shared_ptr<Logical> expr) override {
    any left = evaluate(expr->left);

    if (expr->op.type == OR) {
      if (isTruthy(left)) return left;
    } else {
      if (!isTruthy(left)) return left;
    }

    return evaluate(expr->right);
  }

  any visitSetExpr(shared_ptr<Set> expr) override {
    any object = evaluate(expr->object);

    if (object.type() != typeid(shared_ptr<LoxInstance>)) {
      throw RuntimeError(expr->name,
                         "Only instances have fields.");
    }

    any value = evaluate(expr->value);
    any_cast<
        shared_ptr<LoxInstance>>(object)->set(expr->name, value);
    return value;
  }

  any visitSuperExpr(shared_ptr<Super> expr) override {
    int distance = locals[expr];
    auto superclass = any_cast<
        shared_ptr<LoxClass>>(environment->getAt(
            distance, "super"));

    auto object = any_cast<shared_ptr<LoxInstance>>(
        environment->getAt(distance - 1, "this"));

    shared_ptr<LoxFunction> method = superclass->findMethod(
        expr->method.lexeme);

    if (method == nullptr) {
      throw RuntimeError(expr->method,
          "Undefined property '" + expr->method.lexeme + "'.");
    }

    return method->bind(object);
  }

  any visitThisExpr(shared_ptr<This> expr) override {
    return lookUpVariable(expr->keyword, expr);
  }

  any visitUnaryExpr(shared_ptr<Unary> expr) override {
    any right = evaluate(expr->right);

    switch (expr->op.type) {
      case BANG:
        return !isTruthy(right);
      case MINUS:
        checkNumberOperand(expr->op, right);
        return -any_cast<double>(right);
    }

    // Unreachable.
    return {};
  }

  any visitVariableExpr(
      shared_ptr<Variable> expr) override {
    return lookUpVariable(expr->name, expr);
  }

private:
  any lookUpVariable(const Token& name,
                          shared_ptr<Expr> expr) {
    auto elem = locals.find(expr);
    if (elem != locals.end()) {
      int distance = elem->second;
      return environment->getAt(distance, name.lexeme);
    } else {
      return globals->get(name);
    }
  }

  void checkNumberOperand(const Token& op,
                          const any& operand) {
    if (operand.type() == typeid(double)) return;
    throw RuntimeError{op, "Operand must be a number."};
  }

  void checkNumberOperands(const Token& op,
                           const any& left,
                           const any& right) {
    if (left.type() == typeid(double) &&
        right.type() == typeid(double)) {
      return;
    }

    throw RuntimeError{op, "Operands must be numbers."};
  }

  bool isTruthy(const any& object) {
    if (object.type() == typeid(nullptr)) return false;
    if (object.type() == typeid(bool)) {
      return any_cast<bool>(object);
    }
    return true;
  }

  bool isEqual(const any& a, const any& b) {
    if (a.type() == typeid(nullptr) && b.type() == typeid(nullptr)) {
      return true;
    }
    if (a.type() == typeid(nullptr)) return false;

    if (a.type() == typeid(string) &&
        b.type() == typeid(string)) {
      return any_cast<string>(a) ==
             any_cast<string>(b);
    }
    if (a.type() == typeid(double) && b.type() == typeid(double)) {
      return any_cast<double>(a) == any_cast<double>(b);
    }
    if (a.type() == typeid(bool) && b.type() == typeid(bool)) {
      return any_cast<bool>(a) == any_cast<bool>(b);
    }

    return false;
  }

  string stringify(const any& object) {
    if (object.type() == typeid(nullptr)) return "nil";

    if (object.type() == typeid(double)) {
      string text = to_string(
          any_cast<double>(object));
      if (text[text.length() - 2] == '.' &&
          text[text.length() - 1] == '0') {
        text = text.substr(0, text.length() - 2);
      }
      return text;
    }

    if (object.type() == typeid(string)) {
      return any_cast<string>(object);
    }
    if (object.type() == typeid(bool)) {
      return any_cast<bool>(object) ? "true" : "false";
    }
    if (object.type() == typeid(shared_ptr<LoxFunction>)) {
      return any_cast<
          shared_ptr<LoxFunction>>(object)->toString();
    }
    // if (object.type() == typeid(shared_ptr<NativeClock>)) {
    //   return any_cast<
    //       shared_ptr<NativeClock>>(object)->toString();
    // }
    if (object.type() == typeid(shared_ptr<LoxClass>)) {
      return any_cast<
          shared_ptr<LoxClass>>(object)->toString();
    }
    if (object.type() == typeid(shared_ptr<LoxInstance>)) {
      return any_cast<
          shared_ptr<LoxInstance>>(object)->toString();
    }

    return "Error in stringify: object type not recognized.";
  }
};

