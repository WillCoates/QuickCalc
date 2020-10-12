#include "parser.hpp"
#include <sstream>
#include <type_traits>

using namespace quickcalc;

Parser::Parser(ILexer &lexer): _lexer(lexer) {
}

std::unique_ptr<StmtNode> Parser::parse() {
    std::unique_ptr<StmtNode> stmt;
    Token tok = _lexer.peek();
    if (tok.type == TokenType::KEYWORD) {
        switch (std::get<Keyword>(tok.data)) {
        case Keyword::LET:
            stmt = funcDef();
            break;
        default:
            throw std::runtime_error(generateError("Unexpected keyword", tok));
        }
    } else {
        stmt = exprStmt();
    }

    tok = _lexer.read();
    if (tok.type != TokenType::END_OF_STMT) {
        throw std::runtime_error(generateError("Parsing terminated before end of statement", tok));
    }

    return stmt;
}

std::unique_ptr<StmtNode> Parser::exprStmt() {
    return std::make_unique<ExprStmtNode>(additive());
}

std::unique_ptr<StmtNode> Parser::funcDef() {
    Token tok = _lexer.read();
    if (tok.type != TokenType::KEYWORD || std::get<Keyword>(tok.data) != Keyword::LET) {
        throw std::runtime_error(generateError("Expected let statement", tok));
    }

    Token name = _lexer.read();
    if (name.type != TokenType::NAME) {
        throw std::runtime_error(generateError("Expected name", name));
    }

    std::vector<std::string> paramNames;
    tok = _lexer.peek();

    if (tok.type != TokenType::SYMBOL || (std::get<Symbol>(tok.data) != Symbol::BRACKET_OPEN
        && std::get<Symbol>(tok.data) != Symbol::EQUALS)) {
        throw std::runtime_error(generateError("Expected ( or =", tok));
    }

    if (std::get<Symbol>(tok.data) == Symbol::BRACKET_OPEN) {
        _lexer.read();
        do {
            tok = _lexer.read();
            if (tok.type != TokenType::NAME) {
                throw std::runtime_error(generateError("Expected name", tok));
            }
            paramNames.push_back(std::get<std::string>(tok.data));
            tok = _lexer.read();
            if (tok.type != TokenType::SYMBOL) {
                throw std::runtime_error(generateError("Expected , or )", tok));
            }
        } while (std::get<Symbol>(tok.data) != Symbol::BRACKET_CLOSE);
    }

    paramNames.shrink_to_fit();

    tok = _lexer.read();

    if (tok.type != TokenType::SYMBOL || std::get<Symbol>(tok.data) != Symbol::EQUALS) {
        throw std::runtime_error(generateError("Expected =", tok));
    }

    auto expr = additive();
    return std::make_unique<FuncDefNode>(std::get<std::string>(name.data), std::move(expr), std::move(paramNames));
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
        case Symbol::COMMA:
            return lhs;
        }
    default:
        throw std::runtime_error(generateError("Expected + or -", op));
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
    Token tok = _lexer.peek();
    switch (tok.type) {
    case TokenType::SYMBOL:
        switch (std::get<Symbol>(tok.data)) {
        case Symbol::BRACKET_OPEN:
            _lexer.read();
            return brackets();
        case Symbol::ADD:
            _lexer.read();
            return expression();
        case Symbol::SUBTRACT:
            _lexer.read();
            return std::make_unique<UnaryOperationNode>(UnaryOperation::NEGATE, expression());
        default:
            throw std::runtime_error(generateError("Expected ( + or -", tok));
        }
    case TokenType::NUMBER:
        _lexer.read();
        return std::make_unique<ConstNode>(std::get<double>(tok.data));
    case TokenType::NAME:
        return funcCall();
    default:
        throw std::runtime_error(generateError("Expected symbol or number", tok));
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

std::unique_ptr<ExprNode> Parser::funcCall() {
    Token name = _lexer.read();
    if (name.type != TokenType::NAME) {
        throw std::runtime_error(generateError("Expected name", name));
    }

    std::vector<std::unique_ptr<ExprNode>> params;

    Token tok = _lexer.peek();
    if (tok.type == TokenType::SYMBOL && std::get<Symbol>(tok.data) == Symbol::BRACKET_OPEN) {
        _lexer.read();
        do {
            params.push_back(additive());
            tok = _lexer.read();
            if (tok.type != TokenType::SYMBOL) {
                throw std::runtime_error(generateError("Expected , or )", tok));
            }
        } while (std::get<Symbol>(tok.data) != Symbol::BRACKET_CLOSE);
    }
    params.shrink_to_fit();

    return std::make_unique<FunctionInvocationNode>(std::get<std::string>(name.data), std::move(params));
}

std::string Parser::generateError(const std::string &msg, const Token &token) {
    std::ostringstream error;
    error << msg << " " << token.type;
    std::visit([&error] (auto &arg) {
        using T = std::decay<decltype(arg)>::type;
        if constexpr (std::is_same<T, Symbol>::value || std::is_same<T, double>::value || std::is_same<T, Keyword>::value) {
            error << " " << arg;
        }
    }, token.data);
    error << " Line " << token.line << " Col " << token.column;
    return error.str();
}
