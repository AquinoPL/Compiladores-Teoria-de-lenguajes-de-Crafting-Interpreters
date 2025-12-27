#include <iostream>
#include <fstream>
#include <sstream>

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
    
    //New token
    INIT,

    END_OF_FILE
};

class Token {
public:
    TokenType type;
    string lexeme;
    string literal;
    int line;

    Token(TokenType type, const string& lexeme, const string& literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}

    string toString() const {
        return tokenTypeToString(type) + " " + lexeme + " " + literal;
    }

    string tokenTypeToString(TokenType type) const {
        switch (type) {
            case LEFT_PAREN: return "LEFT_PAREN";
            case RIGHT_PAREN: return "RIGHT_PAREN";
            case LEFT_BRACE: return "LEFT_BRACE";
            case RIGHT_BRACE: return "RIGHT_BRACE";
            case COMMA: return "COMMA";
            case DOT: return "DOT";
            case MINUS: return "MINUS";
            case PLUS: return "PLUS";
            case SEMICOLON: return "SEMICOLON";
            case SLASH: return "SLASH";
            case STAR: return "STAR";
            case BANG: return "BANG";
            case BANG_EQUAL: return "BANG_EQUAL";
            case EQUAL: return "EQUAL";
            case EQUAL_EQUAL: return "EQUAL_EQUAL";
            case GREATER: return "GREATER";
            case GREATER_EQUAL: return "GREATER_EQUAL";
            case LESS: return "LESS";
            case LESS_EQUAL: return "LESS_EQUAL";
            case IDENTIFIER: return "IDENTIFIER";
            case STRING: return "STRING";
            case NUMBER: return "NUMBER";
            case AND: return "AND";
            case CLASS: return "CLASS";
            case ELSE: return "ELSE";
            case FALSE: return "FALSE";
            case FUN: return "FUN";
            case FOR: return "FOR";
            case IF: return "IF";
            case NIL: return "NIL";
            case OR: return "OR";
            case PRINT: return "PRINT";
            case RETURN: return "RETURN";
            case SUPER: return "SUPER";
            case THIS: return "THIS";
            case TRUE: return "TRUE";
            case VAR: return "VAR";
            case WHILE: return "WHILE";
            case INIT: return "INIT";
            case END_OF_FILE: return "END_OF_FILE";
            default: return "UNKNOWN";
        }
    }
};

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


string readFile(const string& path) {
    ifstream file(path);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    string filename;
    
    cout << "Ingrese el nombre del archivo Lox: ";
    fflush(stdin);getline(cin,filename);

    string source = readFile(filename);
    Scanner scanner(source);
    vector<Token> tokens = scanner.scanTokens();

    int lineNumber = 1;
    cout << lineNumber << ": ";
    for (const Token& token : tokens) {
        if (token.line != lineNumber) {
            cout << endl;
            lineNumber = token.line;
            cout << lineNumber << ": ";
        }
        cout << token.tokenTypeToString(token.type) << " ";
    }
    return 0;
}