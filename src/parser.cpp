#include "parser.hpp"

using namespace quickcalc;

Parser::Parser(ILexer &lexer): _lexer(lexer) {
}

std::unique_ptr<StmtNode> Parser::parse() {
    auto stmt = std::make_unique<ExprStmtNode>(additive());
    Token tok = _lexer.read();
    if (tok.type != TokenType::END_OF_STMT) {
        throw std::runtime_error("Parsing terminated before end of statement");
    }
    return stmt;
}

std::unique_ptr<ExprNode> Parser::additive() {
    std::unique_ptr<ExprNode> lhs = multiplicative();
    Token op = _lexer.peek();
    switch (op.type) {
    case TokenType::SYMBOL:
        switch (std::get<Symbol>(op.data)) {
        case Symbol::ADD:
            _lexer.read();
            return std::make_unique<BinaryOperationNode>(BinaryOperation::ADD, std::move(lhs), additive());

        case Symbol::SUBTRACT:
            _lexer.read();
            return std::make_unique<BinaryOperationNode>(BinaryOperation::SUBTRACT, std::move(lhs), additive());
        
        case Symbol::BRACKET_CLOSE:
            return lhs;
        }
    default:
        throw std::runtime_error("Unexpected token");
    case TokenType::END_OF_STMT:
        _lexer.read();
        return lhs;
    }
}

std::unique_ptr<ExprNode> Parser::multiplicative() {
    std::unique_ptr<ExprNode> lhs = expression();
    Token op = _lexer.peek();
    switch (op.type) {
    case TokenType::SYMBOL:
        switch (std::get<Symbol>(op.data)) {
        case Symbol::MULTIPLY:
            _lexer.read();
            return std::make_unique<BinaryOperationNode>(BinaryOperation::MULTIPLY, std::move(lhs), multiplicative());

        case Symbol::DIVIDE:
            _lexer.read();
            return std::make_unique<BinaryOperationNode>(BinaryOperation::DIVIDE, std::move(lhs), multiplicative());
        }
    default:
        return lhs;
    }
}

std::unique_ptr<ExprNode> Parser::expression() {
    Token op = _lexer.read();
    switch (op.type) {
    case TokenType::SYMBOL:
        switch (std::get<Symbol>(op.data)) {
        case Symbol::BRACKET_OPEN:
            return brackets();
        case Symbol::ADD:
            return expression();
        case Symbol::SUBTRACT:
            return std::make_unique<UnaryOperationNode>(UnaryOperation::NEGATE, expression());
        default:
            throw std::runtime_error("Expected +, - or (");
        }
    case TokenType::NUMBER:
        return std::make_unique<ConstNode>(std::get<double>(op.data));
    default:
        throw std::runtime_error("Expected expression");
    }
}

std::unique_ptr<ExprNode> Parser::brackets() {
    std::unique_ptr<ExprNode> inner = additive();
    Token op = _lexer.read();
    if (op.type != TokenType::SYMBOL || std::get<Symbol>(op.data) != Symbol::BRACKET_CLOSE) {
        throw std::runtime_error("Expected closing bracket");
    }
    return inner;
}
