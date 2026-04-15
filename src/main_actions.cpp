#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "antlr4-runtime.h"
#include "ExampleActionsLexer.h"
#include "ExampleActionsParser.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: main_actions <file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file " << argv[1] << std::endl;
        return 1;
    }

    std::string line;
    int lineNum = 0;
    int errors = 0;

    while (std::getline(file, line)) {
        lineNum++;

        if (line.empty()) continue;

        try {
            antlr4::ANTLRInputStream inputStream(line);
            ExampleActionsLexer lexer(&inputStream);
            antlr4::CommonTokenStream tokens(&lexer);
            ExampleActionsParser parser(&tokens);

            parser.program();

            if (parser.getNumberOfSyntaxErrors() > 0) {
                throw std::runtime_error("Syntax error");
            }

        } catch (const std::exception &e) {
            std::cerr << "Error at line " << lineNum << ": " << e.what() << std::endl;
            errors++;
        }
    }

    file.close();

    if (errors > 0) {
        std::cerr << "Failed with " << errors << " error(s)" << std::endl;
        return 1;
    }

    return 0;
}