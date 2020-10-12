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

TEST(parse, ParseMultiple) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NUMBER, 1.0 },
        { 1, 0, TokenType::END_OF_STMT },
        { 2, 0, TokenType::NUMBER, 2.0 },
    });
    std::unique_ptr<StmtNode> expectedA = std::make_unique<ExprStmtNode>(
        std::make_unique<ConstNode>(1.0)
    );
    std::unique_ptr<StmtNode> expectedB = std::make_unique<ExprStmtNode>(
        std::make_unique<ConstNode>(2.0)
    );
    testParser(lexer, expectedA);
    testParser(lexer, expectedB);
}

TEST(parser, ParseFunctionDefinitionNoArg) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::KEYWORD, Keyword::LET },
        { 1, 0, TokenType::NAME, "foo" },
        { 2, 0, TokenType::SYMBOL, Symbol::EQUALS },
        { 3, 0, TokenType::NUMBER, 1.0 },
        { 4, 0, TokenType::SYMBOL, Symbol::ADD },
        { 5, 0, TokenType::NUMBER, 2.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<FuncDefNode>(
            "foo",
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<ConstNode>(1.0),
                std::make_unique<ConstNode>(2.0)
            ),
            std::vector<std::string>()
        );
    testParser(lexer, expected);
}

TEST(parser, ParseFunctionDefinitionWithSingleArg) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::KEYWORD, Keyword::LET },
        { 1, 0, TokenType::NAME, "foo" },
        { 2, 0, TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { 3, 0, TokenType::NAME, "a" },
        { 4, 0, TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
        { 5, 0, TokenType::SYMBOL, Symbol::EQUALS },
        { 6, 0, TokenType::NUMBER, 1.0 },
        { 7, 0, TokenType::SYMBOL, Symbol::ADD },
        { 8, 0, TokenType::NUMBER, 2.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<FuncDefNode>(
            "foo",
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<ConstNode>(1.0),
                std::make_unique<ConstNode>(2.0)
            ),
            std::vector<std::string>({"a"})
        );
    testParser(lexer, expected);
}

TEST(parser, ParseFunctionDefinitionWithMultiArg) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::KEYWORD, Keyword::LET },
        { 1, 0, TokenType::NAME, "foo" },
        { 2, 0, TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { 3, 0, TokenType::NAME, "a" },
        { 4, 0, TokenType::SYMBOL, Symbol::COMMA },
        { 5, 0, TokenType::NAME, "b" },
        { 6, 0, TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
        { 7, 0, TokenType::SYMBOL, Symbol::EQUALS },
        { 8, 0, TokenType::NUMBER, 1.0 },
        { 9, 0, TokenType::SYMBOL, Symbol::ADD },
        { 10, 0, TokenType::NUMBER, 2.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<FuncDefNode>(
            "foo",
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<ConstNode>(1.0),
                std::make_unique<ConstNode>(2.0)
            ),
            std::vector<std::string>({"a", "b"})
        );
    testParser(lexer, expected);
}

TEST(parser, ParseInvokeNoArg) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NAME, "foo" },
        { 1, 0, TokenType::SYMBOL, Symbol::ADD },
        { 2, 0, TokenType::NUMBER, 1.0 },
    });
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<FunctionInvocationNode>("foo", std::vector<std::unique_ptr<ExprNode>>()),
                std::make_unique<ConstNode>(1.0)
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseInvokeSingleArg) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NAME, "foo" },
        { 1, 0, TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { 2, 0, TokenType::NUMBER, 1.0 },
        { 3, 0, TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
        { 4, 0, TokenType::SYMBOL, Symbol::ADD },
        { 5, 0, TokenType::NUMBER, 2.0 },
    });
    std::vector<std::unique_ptr<ExprNode>> params;
    params.push_back(std::make_unique<ConstNode>(1.0));
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<FunctionInvocationNode>("foo", std::move(params)),
                std::make_unique<ConstNode>(2.0)
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseInvokeMultiArg) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NAME, "foo" },
        { 1, 0, TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { 2, 0, TokenType::NUMBER, 1.0 },
        { 3, 0, TokenType::SYMBOL, Symbol::COMMA },
        { 4, 0, TokenType::NUMBER, 2.0 },
        { 5, 0, TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
        { 6, 0, TokenType::SYMBOL, Symbol::ADD },
        { 7, 0, TokenType::NUMBER, 3.0 },
    });
    std::vector<std::unique_ptr<ExprNode>> params;
    params.push_back(std::make_unique<ConstNode>(1.0));
    params.push_back(std::make_unique<ConstNode>(2.0));
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<FunctionInvocationNode>("foo", std::move(params)),
                std::make_unique<ConstNode>(3.0)
            )
        );
    testParser(lexer, expected);
}

TEST(parser, ParseInvokeComplexArg) {
    MockLexer lexer = MockLexer({
        { 0, 0, TokenType::NAME, "foo" },
        { 1, 0, TokenType::SYMBOL, Symbol::BRACKET_OPEN },
        { 2, 0, TokenType::NUMBER, 1.0 },
        { 3, 0, TokenType::SYMBOL, Symbol::ADD },
        { 4, 0, TokenType::NUMBER, 2.0 },
        { 5, 0, TokenType::SYMBOL, Symbol::BRACKET_CLOSE },
        { 6, 0, TokenType::SYMBOL, Symbol::ADD },
        { 7, 0, TokenType::NUMBER, 3.0 },
    });
    std::vector<std::unique_ptr<ExprNode>> params;
    params.push_back(std::make_unique<BinaryOperationNode>(
        BinaryOperation::ADD,
        std::make_unique<ConstNode>(1.0),
        std::make_unique<ConstNode>(2.0)
    ));
    std::unique_ptr<StmtNode> expected =
        std::make_unique<ExprStmtNode>(
            std::make_unique<BinaryOperationNode>(
                BinaryOperation::ADD,
                std::make_unique<FunctionInvocationNode>("foo", std::move(params)),
                std::make_unique<ConstNode>(3.0)
            )
        );
    testParser(lexer, expected);
}
