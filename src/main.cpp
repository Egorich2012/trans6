#include <iostream>
#include <fstream>
#include <string>
#include "antlr4-runtime.h"
#include "ExampleLexer.h"
#include "ExampleParser.h"
#include "ExampleInterpreter.h"

using namespace antlr4;

// обработчик синтаксических ошибок
class BailErrorListener : public BaseErrorListener {
public:
    void syntaxError(Recognizer *recognizer, Token *offendingSymbol,
                     size_t line, size_t charPositionInLine,
                     const std::string &msg, std::exception_ptr e) override {
        std::cerr << "Syntax Error at line " << line << ":" << charPositionInLine
                  << " - " << msg << std::endl;
        exit(1);
    }
};

int main(int argc, const char* argv[]) {
    // проверка аргументов
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

    std::string line;
    int lineNum = 0;
    int errors = 0;

    while (std::getline(stream, line)) {
        lineNum++;

        if (line.empty()) continue;

        try {
            ANTLRInputStream input(line);
            ExampleLexer lexer(&input);

            BailErrorListener errorListener;
            lexer.removeErrorListeners();
            lexer.addErrorListener(&errorListener);

            CommonTokenStream tokens(&lexer);
            ExampleParser parser(&tokens);

            parser.removeErrorListeners();
            parser.addErrorListener(&errorListener);

            // дерево
            tree::ParseTree *tree = parser.program();

            // НАШ visitor (но имя как у него)
            ExampleInterpreter visitor;
            visitor.visit(tree);

        } catch (const std::exception& e) {
            std::cerr << "Error at line " << lineNum << ": " << e.what() << std::endl;
            errors++;
        }
    }

    stream.close();

    if (errors > 0) {
        std::cerr << "Failed with " << errors << " error(s)" << std::endl;
        return 1;
    }

    return 0;
}