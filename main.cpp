#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

#include "Tokenizer.h"
#include "Parser.h"
#include "Interpreter.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: miniscript <source-file>" << std::endl;
        return 1;
    }

    // Read entire source file into a string
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // Tokenize
    Tokenizer tokenizer(source);
    std::vector<Token> tokens;
    while (true) {
        Token token = tokenizer.getNextToken();
        tokens.push_back(token);
        if (token.type == TokenType::EndOfFile) break;
    }

    // Parse
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements;
    try {
        statements = parser.parse();
    } catch (const std::runtime_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return 1;
    }

    // Interpret
    Interpreter interpreter;
    interpreter.interpret(statements);

    return 0;
}
