#include "parser.hpp"
#include <sstream>
#include <type_traits>

using namespace quickcalc;

Parser::Parser(ILexer &lexer): _lexer(lexer) {
}

std::unique_ptr<StmtNode> Parser::parse() {
    auto stmt = std::make_unique<ExprStmtNode>(additive());
    Token tok = _lexer.read();
    if (tok.type != TokenType::END_OF_STMT) {
        throw std::runtime_error(generateError("Parsing terminated before end of statement", tok));
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
        throw std::runtime_error(generateError("Expected + or - (or ')' if in brackets)", op));
    case TokenType::END_OF_STMT:
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
            throw std::runtime_error(generateError("Expected ( + or -", op));
        }
    case TokenType::NUMBER:
        return std::make_unique<ConstNode>(std::get<double>(op.data));
    default:
        throw std::runtime_error(generateError("Expected symbol or number", op));
    }
}

std::unique_ptr<ExprNode> Parser::brackets() {
    std::unique_ptr<ExprNode> inner = additive();
    Token op = _lexer.read();
    if (op.type != TokenType::SYMBOL || std::get<Symbol>(op.data) != Symbol::BRACKET_CLOSE) {
        throw std::runtime_error(generateError("Expected closing bracket", op));
    }
    return inner;
}

std::string Parser::generateError(const std::string &msg, const Token &token) {
    std::ostringstream error;
    error << msg << " " << token.type;
    std::visit([&error] (auto &arg) {
        using T = std::decay<decltype(arg)>::type;
        if constexpr (std::is_same<T, Symbol>::value || std::is_same<T, double>::value) {
            error << " " << arg;
        }
    }, token.data);
    error << " Line " << token.line << " Col " << token.column;
    return error.str();
}
