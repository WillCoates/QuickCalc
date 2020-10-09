#include <iostream>
#include <sstream>
#include <stdexcept>
#include "lexer.hpp"
#include "parser.hpp"
#include "executor.hpp"

using namespace quickcalc;

int main(int argc, char *argv[]) {
    std::cout << "QuickCalc" << std::endl;
    std::stringstream argInput;
    std::istream *input = &std::cin;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            argInput << argv[i] << " ";
        }
        input = &argInput;
    }
    
    Lexer lex = Lexer(*input);
    Parser parser = Parser(lex);
    Executor executor;

    while (!input->eof()) {
        try {
            auto ast = parser.parse();
            ast->accept(executor);
            std::cout << "Result = " << executor.lastResult() << std::endl;
        } catch (std::runtime_error &e) {
            std::cout << "Exception: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}
