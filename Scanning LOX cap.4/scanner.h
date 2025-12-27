#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <map>
#include "token.h"

using namespace std;

class Scanner {
public:
    Scanner(const string& source) : source(source) {}

    vector<Token> scanTokens() {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }

        tokens.push_back(Token(TokenType::END_OF_FILE, "", "", line));
        return tokens;
    }
    
private:
    string source;
    vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    map<string, TokenType> keywords = {
        {"and", TokenType::AND},
        {"class", TokenType::CLASS},
        {"else", TokenType::ELSE},
        {"false", TokenType::FALSE},
        {"for", TokenType::FOR},
        {"fun", TokenType::FUN},
        {"if", TokenType::IF},
        {"nil", TokenType::NIL},
        {"or", TokenType::OR},
        {"print", TokenType::PRINT},
        {"return", TokenType::RETURN},
        {"super", TokenType::SUPER},
        {"this", TokenType::THIS},
        {"true", TokenType::TRUE},
        {"var", TokenType::VAR},
        {"while", TokenType::WHILE},
        {"init", TokenType::INIT}
    };

    bool isAtEnd() const {
        return current >= source.length();
    }

    void scanToken() {
        char c = advance();
        switch (c) {
            case '(': addToken(TokenType::LEFT_PAREN); break;
            case ')': addToken(TokenType::RIGHT_PAREN); break;
            case '{': addToken(TokenType::LEFT_BRACE); break;
            case '}': addToken(TokenType::RIGHT_BRACE); break;
            case ',': addToken(TokenType::COMMA); break;
            case '.': addToken(TokenType::DOT); break;
            case '-': addToken(TokenType::MINUS); break;
            case '+': addToken(TokenType::PLUS); break;
            case ';': addToken(TokenType::SEMICOLON); break;
            case '*': addToken(TokenType::STAR); break;
            case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
            case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
            case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
            case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    addToken(TokenType::SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;
            case '\n':
                line++;
                break;
            case '"': stringLiteral(); break;
            default:
                if (isDigit(c)) {
                    number();
                } else if (isAlpha(c)) {
                    identifier();
                } else {
                    cout<< "Unexpected character " << line << ": " << c << endl;
                }
                break;
        }
    }


    char advance() {
        return source[current++];
    }

    void addToken(TokenType type) {
        addToken(type, "");
    }

    void addToken(TokenType type, const string& literal) {
        string text = source.substr(start, current - start);
        tokens.push_back(Token(type, text, literal, line));
    }

    bool match(char expected) {
        if (isAtEnd()) return false;
        if (source[current] != expected) return false;
        current++;
        return true;
    }

    char peek() const {
        if (isAtEnd()) return '\0';
        return source[current];
    }

    char peekNext() const {
        if (current + 1 >= source.length()) return '\0';
        return source[current + 1];
    }

    void stringLiteral() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') line++;
            advance();
        }

        if (isAtEnd()) {
            cerr << "Unterminated string at line " << line << endl;
            return;
        }

        advance();

        string value = source.substr(start + 1, current - start - 2);
        addToken(TokenType::STRING, value);
    }

    void number() {
        while (isDigit(peek())) advance();

        if (peek() == '.' && isDigit(peekNext())) {
            advance();

            while (isDigit(peek())) advance();
        }

        addToken(TokenType::NUMBER, source.substr(start, current - start));
    }

    void identifier() {
        while (isAlphaNumeric(peek())) advance();

        string text = source.substr(start, current - start);
        TokenType type = TokenType::IDENTIFIER;
        if (keywords.find(text) != keywords.end()) {
            type = keywords[text];
        }
        addToken(type);
    }

    bool isDigit(char c) const {
        return c >= '0' && c <= '9';
    }

    bool isAlpha(char c) const {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
                c == '_';
    }

    bool isAlphaNumeric(char c) const {
        return isAlpha(c) || isDigit(c);
    }
};

#endif