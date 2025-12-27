#pragma once

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "Error.h"
#include "Expr.h"
#include "Stmt.h"
#include "Token.h"

using namespace std;

class Parser {
  struct ParseError: public runtime_error {
    using runtime_error::runtime_error;
  };

  const vector<Token>& tokens;
  int current = 0;

public:
  Parser(const vector<Token>& tokens)
    : tokens{tokens}
  {}

  vector<shared_ptr<Stmt>> parse() {
    vector<shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
      statements.push_back(declaration());
    }

    return statements;
  }

private:
  shared_ptr<Expr> expression() {
    return assignment();
  }

  shared_ptr<Stmt> declaration() {
    try {
      if (match(CLASS)) return classDeclaration();
      if (match(FUN)) return function("function");
      if (match(VAR)) return varDeclaration();

      return statement();
    } catch (ParseError error) {
      synchronize();
      return nullptr;
    }
  }

  shared_ptr<Stmt> classDeclaration() {
    Token name = consume(IDENTIFIER, "Expect class name.");

    shared_ptr<Variable> superclass = nullptr;
    if (match(LESS)) {
      consume(IDENTIFIER, "Expect superclass name.");
      superclass = make_shared<Variable>(previous());
    }

    consume(LEFT_BRACE, "Expect '{' before class body.");

    vector<shared_ptr<Function>> methods;
    while (!check(RIGHT_BRACE) && !isAtEnd()) {
      methods.push_back(function("method"));
    }

    consume(RIGHT_BRACE, "Expect '}' after class body");

    return make_shared<Class>(move(name),
                                   superclass,
                                   move(methods));
  }

  shared_ptr<Stmt> statement() {
    if (match(FOR)) return forStatement();
    if (match(IF)) return ifStatement();
    if (match(PRINT)) return printStatement();
    if (match(RETURN)) return returnStatement();
    if (match(WHILE)) return whileStatement();
    if (match(LEFT_BRACE)) return make_shared<Block>(block());

    return expressionStatement();
  }

  shared_ptr<Stmt> forStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'for'.");

    shared_ptr<Stmt> initializer;
    if (match(SEMICOLON)) {
      initializer = nullptr;
    } else if (match(VAR)) {
      initializer = varDeclaration();
    } else {
      initializer = expressionStatement();
    }

    shared_ptr<Expr> condition = nullptr;
    if (!check(SEMICOLON)) {
      condition = expression();
    }
    consume(SEMICOLON, "Expect ';' after loop condition.");

    shared_ptr<Expr> increment = nullptr;
    if (!check(RIGHT_PAREN)) {
      increment = expression();
    }
    consume(RIGHT_PAREN, "Expect ')' after for clauses.");
    shared_ptr<Stmt> body = statement();

    if (increment != nullptr) {
      body = make_shared<Block>(
          vector<shared_ptr<Stmt>>{
              body,
              make_shared<Expression>(increment)});
    }

    if (condition == nullptr) {
      condition = make_shared<Literal>(true);
    }
    body = make_shared<While>(condition, body);

    if (initializer != nullptr) {
      body = make_shared<Block>(
          vector<shared_ptr<Stmt>>{initializer, body});
    }

    return body;
  }

  shared_ptr<Stmt> ifStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    shared_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after if condition.");

    shared_ptr<Stmt> thenBranch = statement();
    shared_ptr<Stmt> elseBranch = nullptr;
    if (match(ELSE)) {
      elseBranch = statement();
    }

    return make_shared<If>(condition, thenBranch, elseBranch);
  }

  shared_ptr<Stmt> printStatement() {
    shared_ptr<Expr> value = expression();
    consume(SEMICOLON, "Expect ';' after value.");
    return make_shared<Print>(value);
  }

  shared_ptr<Stmt> returnStatement() {
    Token keyword = previous();
    shared_ptr<Expr> value = nullptr;
    if (!check(SEMICOLON)) {
      value = expression();
    }

    consume(SEMICOLON, "Expect ';' after return value.");
    return make_shared<Return>(keyword, value);
  }

  shared_ptr<Stmt> varDeclaration() {
    Token name = consume(IDENTIFIER, "Expect variable name.");

    shared_ptr<Expr> initializer = nullptr;
    if (match(EQUAL)) {
      initializer = expression();
    }

    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return make_shared<Var>(move(name), initializer);
  }

  shared_ptr<Stmt> whileStatement() {
    consume(LEFT_PAREN, "Expect '(' after 'while'.");
    shared_ptr<Expr> condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after condition.");
    shared_ptr<Stmt> body = statement();

    return make_shared<While>(condition, body);
  }

  shared_ptr<Stmt> expressionStatement() {
    shared_ptr<Expr> expr = expression();
    consume(SEMICOLON, "Expect ';' after expression.");
    return make_shared<Expression>(expr);
  }

  shared_ptr<Function> function(string kind) {
    Token name = consume(IDENTIFIER, "Expect " + kind + " name.");
    consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
    vector<Token> parameters;
    if (!check(RIGHT_PAREN)) {
      do {
        if (parameters.size() >= 255) {
          error(peek(), "Can't have more than 255 parameters.");
        }

        parameters.push_back(
            consume(IDENTIFIER, "Expect parameter name."));
      } while (match(COMMA));
    }
    consume(RIGHT_PAREN, "Expect ')' after parameters.");

    consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
    vector<shared_ptr<Stmt>> body = block();
    return make_shared<Function>(move(name),
                                      move(parameters),
                                      move(body));
  }

  vector<shared_ptr<Stmt>> block() {
    vector<shared_ptr<Stmt>> statements;

    while (!check(RIGHT_BRACE) && !isAtEnd()) {
      statements.push_back(declaration());
    }

    consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
  }

  shared_ptr<Expr> assignment() {
    shared_ptr<Expr> expr = orExpression();

    if (match(EQUAL)) {
      Token equals = previous();
      shared_ptr<Expr> value = assignment();

      if (Variable* e = dynamic_cast<Variable*>(expr.get())) {
        Token name = e->name;
        return make_shared<Assign>(move(name), value);
      } else if (Get* get = dynamic_cast<Get*>(expr.get())) {
        return make_shared<Set>(get->object, get->name, value);
      }

      error(move(equals), "Invalid assignment target.");
    }

    return expr;
  }

  shared_ptr<Expr> orExpression() {
    shared_ptr<Expr> expr = andExpression();

    while (match(OR)) {
      Token op = previous();
      shared_ptr<Expr> right = andExpression();
      expr = make_shared<Logical>(expr, move(op), right);
    }

    return expr;
  }

  shared_ptr<Expr> andExpression() {
    shared_ptr<Expr> expr = equality();

    while (match(AND)) {
      Token op = previous();
      shared_ptr<Expr> right = equality();
      expr = make_shared<Logical>(expr, move(op), right);
    }

    return expr;
  }

  shared_ptr<Expr> equality() {
    shared_ptr<Expr> expr = comparison();

    while (match(BANG_EQUAL, EQUAL_EQUAL)) {
      Token op = previous();
      shared_ptr<Expr> right = comparison();
      expr = make_shared<Binary>(expr, move(op), right);
    }

    return expr;
  }

  shared_ptr<Expr> comparison() {
    shared_ptr<Expr> expr = term();

    while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
      Token op = previous();
      shared_ptr<Expr> right = term();
      expr = make_shared<Binary>(expr, move(op), right);
    }

    return expr;
  }

  shared_ptr<Expr> term() {
    shared_ptr<Expr> expr = factor();

    while (match(MINUS, PLUS)) {
      Token op = previous();
      shared_ptr<Expr> right = factor();
      expr = make_shared<Binary>(expr, move(op), right);
    }

    return expr;
  }

  shared_ptr<Expr> factor() {
    shared_ptr<Expr> expr = unary();

    while (match(SLASH, STAR)) {
      Token op = previous();
      shared_ptr<Expr> right = unary();
      expr = make_shared<Binary>(expr, move(op), right);
    }

    return expr;
  }

  shared_ptr<Expr> unary() {
    if (match(BANG, MINUS)) {
      Token op = previous();
      shared_ptr<Expr> right = unary();
      return make_shared<Unary>(move(op), right);
    }

    return call();
  }

  shared_ptr<Expr> finishCall(shared_ptr<Expr> callee) {
    vector<shared_ptr<Expr>> arguments;
    if (!check(RIGHT_PAREN)) {
      do {
        if (arguments.size() >= 255) {
          error(peek(), "Can't have more than 255 arguments.");
        }
        arguments.push_back(expression());
      } while (match(COMMA));
    }

    Token paren = consume(RIGHT_PAREN,
                          "Expect ')' after arguments.");

    return make_shared<Call>(callee,
                                  move(paren),
                                  move(arguments));
  }

  shared_ptr<Expr> call() {
    shared_ptr<Expr> expr = primary();

    while (true) {
      if (match(LEFT_PAREN)) {
        expr = finishCall(expr);
      } else if (match(DOT)) {
        Token name = consume(IDENTIFIER,
            "Expect property name after '.'.");
        expr = make_shared<Get>(expr, move(name));
      } else {
        break;
      }
    }

    return expr;
  }

  shared_ptr<Expr> primary() {
    if (match(FALSE)) return make_shared<Literal>(false);
    if (match(TRUE)) return make_shared<Literal>(true);
    if (match(NIL)) return make_shared<Literal>(nullptr);

    if (match(NUMBER, STRING)) {
      return make_shared<Literal>(previous().literal);
    }

    if (match(SUPER)) {
      Token keyword = previous();
      consume(DOT, "Expect '.' after 'super'.");
      Token method = consume(IDENTIFIER,
          "Expect superclass method name.");
      return make_shared<Super>(move(keyword),
                                     move(method));
    }

    if (match(THIS)) return make_shared<This>(previous());

    if (match(IDENTIFIER)) {
      return make_shared<Variable>(previous());
    }

    if (match(LEFT_PAREN)) {
      shared_ptr<Expr> expr = expression();
      consume(RIGHT_PAREN, "Expect ')' after expression.");
      return make_shared<Grouping>(expr);
    }

    throw error(peek(), "Expect expression.");
  }

  template <class... T>
  bool match(T... type) {
    assert((... && is_same_v<T, TokenType>));

    if ((... || check(type))) {
      advance();
      return true;
    }

    return false;
  }

  Token consume(TokenType type, string_view message) {
    if (check(type)) return advance();

    throw error(peek(), message);
  }

  bool check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
  }

  Token advance() {
    if (!isAtEnd()) ++current;
    return previous();
  }

  bool isAtEnd() {
    return peek().type == END_OF_FILE;
  }

  Token peek() {
    return tokens.at(current);
  }

  Token previous() {
    return tokens.at(current - 1);
  }

  ParseError error(const Token& token, string_view message) {
    ::error(token, message);
    return ParseError{""};
  }

  void synchronize() {
    advance();

    while (!isAtEnd()) {
      if (previous().type == SEMICOLON) return;

      switch (peek().type) {
        case CLASS:
        case FUN:
        case VAR:
        case FOR:
        case IF:
        case WHILE:
        case PRINT:
        case RETURN:
          return;
      }

      advance();
    }
  }
};