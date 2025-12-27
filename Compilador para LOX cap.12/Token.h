#pragma once

#include <string>
#include <any>
#include <string>
#include <utility> 
using namespace std; 

enum TokenType {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two character tokens.
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  // Literals.
  IDENTIFIER, STRING, NUMBER,

  // Keywords.
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  END_OF_FILE
};

string toString(TokenType type) {
  static const string strings[] = {
    "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
    "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",
    "BANG", "BANG_EQUAL",
    "EQUAL", "EQUAL_EQUAL",
    "GREATER", "GREATER_EQUAL",
    "LESS", "LESS_EQUAL",
    "IDENTIFIER", "STRING", "NUMBER",
    "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR",
    "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",
    "END_OF_FILE"
  };

  return strings[static_cast<int>(type)];
}

class Token {
public:
  const TokenType type;
  const string lexeme;
  const any literal;
  const int line;

  Token(TokenType type, string lexeme, any literal,
        int line)
    : type{type}, lexeme{move(lexeme)},
      literal{move(literal)}, line{line}
  {}

  string toString() const {
    string literal_text;

    switch (type) {
      case (IDENTIFIER):
        literal_text = lexeme;
        break;
      case (STRING):
        literal_text = any_cast<string>(literal);
        break;
      case (NUMBER):
        literal_text = to_string(any_cast<double>(literal));
        break;
      case (TRUE):
        literal_text = "true";
        break;
      case (FALSE):
        literal_text = "false";
        break;
      default:
        literal_text = "nil";
    }

    return ::toString(type) + " " + lexeme + " " + literal_text;
  }
};