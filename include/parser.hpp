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
        std::unique_ptr<StmtNode> exprStmt();
        std::unique_ptr<StmtNode> funcDef();
        std::unique_ptr<ExprNode> additive();
        std::unique_ptr<ExprNode> multiplicative();
        std::unique_ptr<ExprNode> expression();
        std::unique_ptr<ExprNode> brackets();
        std::unique_ptr<ExprNode> funcCall();

        std::string generateError(const std::string &message, const Token &token);
    };
}
