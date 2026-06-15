#include <fstream>
#include <iostream>
#include <sstream>
#include <any>
#include "antlr4-runtime.h"
#include "ExampleLexer.h"
#include "ExampleParser.h"
#include "Interpreter.h"

class MyErrorListener : public antlr4::BaseErrorListener {
public:
    bool hasErrors = false;
    void syntaxError(antlr4::Recognizer* recognizer,
                     antlr4::Token* offendingSymbol,
                     size_t line, size_t charPositionInLine,
                     const std::string& msg,
                     std::exception_ptr e) override {
        hasErrors = true;
        std::cerr << "Syntax error at line " << line << ": " << msg << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: main <file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    antlr4::ANTLRInputStream input(buffer.str());
    ExampleLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    ExampleParser parser(&tokens);

    MyErrorListener errListener;
    lexer.removeErrorListeners();
    parser.removeErrorListeners();
    lexer.addErrorListener(&errListener);
    parser.addErrorListener(&errListener);

    auto* tree = parser.program();

    // Жёсткая проверка ошибок ДО вычисления
    if (errListener.hasErrors || parser.getNumberOfSyntaxErrors() > 0) {
        return 1;
    }

    if (!tree) return 1;

    try {
        Interpreter interpreter;
        interpreter.visit(tree);
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
