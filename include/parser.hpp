#pragma once
#include "lexer.hpp"
#include "ast.hpp"

namespace quickcalc {
    class Parser {
        ILexer &_lexer;

    public:
        Parser(ILexer &lexer);
        std::unique_ptr<StmtNode> parse();

    private:
        std::unique_ptr<ExprNode> additive();
        std::unique_ptr<ExprNode> multiplicative();
        std::unique_ptr<ExprNode> expression();
        std::unique_ptr<ExprNode> brackets();
    };
}
