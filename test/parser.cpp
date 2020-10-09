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
        { TokenType::NUMBER, 1.0 }
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<ConstNode>(1.0)
        );
    testParser(lexer, expected);
}

TEST(parser, ParseAddition) {
    MockLexer lexer = MockLexer({
        { TokenType::NUMBER, 1.0 },
        { TokenType::SYMBOL, Symbol::ADD },
        { TokenType::NUMBER, 2.0 },
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
        { TokenType::NUMBER, 1.0 },
        { TokenType::SYMBOL, Symbol::SUBTRACT },
        { TokenType::NUMBER, 2.0 },
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
        { TokenType::NUMBER, 1.0 },
        { TokenType::SYMBOL, Symbol::MULTIPLY },
        { TokenType::NUMBER, 2.0 },
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
        { TokenType::NUMBER, 1.0 },
        { TokenType::SYMBOL, Symbol::DIVIDE },
        { TokenType::NUMBER, 2.0 },
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
        { TokenType::NUMBER, 1.0 },
        { TokenType::SYMBOL, Symbol::ADD },
        { TokenType::NUMBER, 2.0 },
        { TokenType::SYMBOL, Symbol::ADD },
        { TokenType::NUMBER, 3.0 },
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
        { TokenType::NUMBER, 1.0 },
        { TokenType::SYMBOL, Symbol::ADD },
        { TokenType::NUMBER, 2.0 },
        { TokenType::SYMBOL, Symbol::MULTIPLY },
        { TokenType::NUMBER, 3.0 },
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
        { TokenType::NUMBER, 1.0 },
        { TokenType::SYMBOL, Symbol::MULTIPLY },
        { TokenType::NUMBER, 2.0 },
        { TokenType::SYMBOL, Symbol::ADD },
        { TokenType::NUMBER, 3.0 },
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
        { TokenType::NUMBER, 1.0 },
        { TokenType::SYMBOL, Symbol::MULTIPLY },
        { TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { TokenType::NUMBER, 2.0 },
        { TokenType::SYMBOL, Symbol::ADD },
        { TokenType::NUMBER, 3.0 },
        { TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
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
        { TokenType::SYMBOL, Symbol::SUBTRACT },
        { TokenType::NUMBER, 1.0 },
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
        { TokenType::SYMBOL, Symbol::SUBTRACT },
        { TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { TokenType::NUMBER, 1.0 },
        { TokenType::SYMBOL, Symbol::ADD },
        { TokenType::NUMBER, 2.0 },
        { TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
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

TEST(parse, SequentalNumberFail) {
    MockLexer lexer = MockLexer({
        { TokenType::NUMBER, 1.0 },
        { TokenType::NUMBER, 2.0 },
    });
    testParserThrows(lexer);
}

TEST(parse, RogueClosedBracketFail) {
    MockLexer lexer = MockLexer({
        { TokenType::NUMBER, 1.0 },
        { TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
    });
    testParserThrows(lexer);
}

TEST(parse, ForgotCloseBracketFail) {
    MockLexer lexer = MockLexer({
        { TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { TokenType::NUMBER, 1.0 },
    });
    testParserThrows(lexer);
}
