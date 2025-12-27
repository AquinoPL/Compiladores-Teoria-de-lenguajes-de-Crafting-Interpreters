#include <iostream> 
#include <string>
#include <vector>
#include <cstring>      
#include <fstream>
#include <sstream>      
#include "Error.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Resolver.h"
#include "Scanner.h"
#include "LoxFunction.cpp"
#include "LoxClass.cpp"
#include "LoxInstance.cpp" 
using namespace std;

string readFile(const string& path) {
    ifstream file(path);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <archivo Lox>" << endl;
        return 1;
    }

    string filename = argv[1];
    string source = readFile(filename);

    Scanner scanner(source);
    vector<Token> tokens = scanner.scanTokens();

    Parser parser(tokens);
    vector<shared_ptr<Stmt>> statements = parser.parse();

    if (!statements.empty()) {
        Interpreter interpreter;
        interpreter.interpret(statements);
    } else {
        cout << "Error en el parseo del archivo." << endl;
    }

    return 0;
}