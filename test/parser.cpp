#include <gtest/gtest.h>
#include <vector>
#include "parser.hpp"

using namespace quickcalc;

namespace {
    class MockLexer: public ILexer {
        std::vector<Token> _tokens;
        int _offset;
    public:
        MockLexer(std::vector<Token> &&tokens): _tokens(std::move(tokens)), _offset(0) {
        }

        Token read() override {
            if (_offset == _tokens.size()) {
                return {};
            }
            return _tokens[_offset++];
        }

        Token peek() override {
            if (_offset == _tokens.size()) {
                return {};
            }
            return _tokens[_offset];
        }
    };

    void testParser(MockLexer &lexer, std::unique_ptr<StmtNode> &expected) {
        Parser parser(lexer);
        std::unique_ptr<StmtNode> actual = parser.parse();
        EXPECT_EQ(*actual, *expected);
    }

    void testParserThrows(MockLexer &lexer) {
        Parser parser(lexer);
        EXPECT_THROW(parser.parse(), std::runtime_error);
    }
}

TEST(parser, ParseNumber) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 }
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<ConstNode>(1.0)
        );
    testParser(lexer, expected);
}

TEST(parser, ParseAddition) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::SYMBOL, Symbol::ADD },
        { 2, 0, TokenType::NUMBER, 2.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<ConstNode>(1.0),
                std::make_unique<ConstNode>(2.0)
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseSubtraction) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::SYMBOL, Symbol::SUBTRACT },
        { 2, 0, TokenType::NUMBER, 2.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::SUBTRACT,
                std::make_unique<ConstNode>(1.0),
                std::make_unique<ConstNode>(2.0)
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseMultiplication) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::SYMBOL, Symbol::MULTIPLY },
        { 2, 0, TokenType::NUMBER, 2.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::MULTIPLY,
                std::make_unique<ConstNode>(1.0),
                std::make_unique<ConstNode>(2.0)
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseDivision) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::SYMBOL, Symbol::DIVIDE },
        { 2, 0, TokenType::NUMBER, 2.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::DIVIDE,
                std::make_unique<ConstNode>(1.0),
                std::make_unique<ConstNode>(2.0)
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseMultipleAddition) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::SYMBOL, Symbol::ADD },
        { 2, 0, TokenType::NUMBER, 2.0 },
        { 3, 0, TokenType::SYMBOL, Symbol::ADD },
        { 4, 0, TokenType::NUMBER, 3.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<ConstNode>(1.0),
                std::make_unique<BinaryOperationNode>(
                    BinaryOperation::ADD,
                    std::make_unique<ConstNode>(2.0),
                    std::make_unique<ConstNode>(3.0)
                )
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseAddMul) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::SYMBOL, Symbol::ADD },
        { 2, 0, TokenType::NUMBER, 2.0 },
        { 3, 0, TokenType::SYMBOL, Symbol::MULTIPLY },
        { 4, 0, TokenType::NUMBER, 3.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<ConstNode>(1.0),
                std::make_unique<BinaryOperationNode>(
                    BinaryOperation::MULTIPLY,
                    std::make_unique<ConstNode>(2.0),
                    std::make_unique<ConstNode>(3.0)
                )
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseMulAdd) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::SYMBOL, Symbol::MULTIPLY },
        { 2, 0, TokenType::NUMBER, 2.0 },
        { 3, 0, TokenType::SYMBOL, Symbol::ADD },
        { 4, 0, TokenType::NUMBER, 3.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<BinaryOperationNode>(
                    BinaryOperation::MULTIPLY,
                    std::make_unique<ConstNode>(1.0),
                    std::make_unique<ConstNode>(2.0)
                ),
                std::make_unique<ConstNode>(3.0)
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseBrackets) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::SYMBOL, Symbol::MULTIPLY },
        { 2, 0, TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { 3, 0, TokenType::NUMBER, 2.0 },
        { 4, 0, TokenType::SYMBOL, Symbol::ADD },
        { 5, 0, TokenType::NUMBER, 3.0 },
        { 6, 0, TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::MULTIPLY,
                std::make_unique<ConstNode>(1.0),
                std::make_unique<BinaryOperationNode>(
                    BinaryOperation::ADD,
                    std::make_unique<ConstNode>(2.0),
                    std::make_unique<ConstNode>(3.0)
                )
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseNegation) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::SYMBOL, Symbol::SUBTRACT },
        { 1, 0, TokenType::NUMBER, 1.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<UnaryOperationNode>(
                UnaryOperation::NEGATE,
                std::make_unique<ConstNode>(1.0)
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseNegationWithBrackets) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::SYMBOL, Symbol::SUBTRACT },
        { 1, 0, TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { 2, 0, TokenType::NUMBER, 1.0 },
        { 3, 0, TokenType::SYMBOL, Symbol::ADD },
        { 4, 0, TokenType::NUMBER, 2.0 },
        { 5, 0, TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<UnaryOperationNode>(
                UnaryOperation::NEGATE,
                std::make_unique<BinaryOperationNode>(
                    BinaryOperation::ADD,
                    std::make_unique<ConstNode>(1.0),
                    std::make_unique<ConstNode>(2.0)
                )
            )
        );
    testParser(lexer, expected);
}

TEST(parser, SequentalNumberFail) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::NUMBER, 2.0 },
    });
    testParserThrows(lexer);
}

TEST(parser, RogueClosedBracketFail) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
    });
    testParserThrows(lexer);
}

TEST(parser, ForgotCloseBracketFail) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { 1, 0, TokenType::NUMBER, 1.0 },
    });
    testParserThrows(lexer);
}
