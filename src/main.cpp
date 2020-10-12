#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
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

    std::vector<StmtNode::ptr> vitalNodes;

    while (!input->eof()) {
        try {
            auto ast = parser.parse();
            // Ensure vital nodes are kept in memory
            auto &astRef = ast->canSafeDelete() ? ast : vitalNodes.emplace_back(std::move(ast));

            astRef->accept(executor);
            if (executor.hasResult()) {
                std::cout << "Result = " << executor.lastResult() << std::endl;
            } else {
                std::cout << "OK" << std::endl;
            }
        } catch (std::runtime_error &e) {
            std::cout << "Exception: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}
