#include <iostream>
#include <fstream>
#include "antlr4-runtime.h"
#include "ExampleLexer.h"
#include "ExampleParser.h"
#include "ExampleInterpreter.h"

class BailErrorListener : public antlr4::BaseErrorListener {
public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol,
                     size_t line, size_t charPositionInLine,
                     const std::string &msg, std::exception_ptr e) override {
        std::cerr << "Syntax Error at line " << line << ":" << charPositionInLine
                  << " - " << msg << std::endl;
        exit(1);
    }
};

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::ifstream stream;
    stream.open(argv[1]);
    if (!stream.is_open()) {
        std::cerr << "Error: Cannot open file " << argv[1] << std::endl;
        return 1;
    }

    try {
        antlr4::ANTLRInputStream input(stream);
        ExampleLexer lexer(&input);

        BailErrorListener errorListener;
        lexer.removeErrorListeners();
        lexer.addErrorListener(&errorListener);

        antlr4::CommonTokenStream tokens(&lexer);
        ExampleParser parser(&tokens);
        parser.removeErrorListeners();
        parser.addErrorListener(&errorListener);

        ExampleInterpreter visitor;
        visitor.visit(parser.program());

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    stream.close();
    return 0;
}