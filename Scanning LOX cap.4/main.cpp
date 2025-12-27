#include <iostream>
#include <fstream>
#include <sstream>
#include "scanner.h"
#include "token.h"

using namespace std;

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