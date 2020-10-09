#include <gtest/gtest.h>
#include <sstream>
#include "lexer.hpp"

using namespace quickcalc;

TEST(lexer, BlankSourceEOS) {
    auto stream = std::istringstream("");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::END_OF_STMT);
    // Cast to void to disable nodiscard warning
    ASSERT_NO_THROW(static_cast<void>(std::get<std::monostate>(tok.data)));
}

TEST(lexer, ReadSingleDigitNumber) {
    auto stream = std::istringstream("1");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    double val;
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 1.0);
}

TEST(lexer, ReadMultiDigitNumber) {
    auto stream = std::istringstream("10");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    double val;
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 10.0);
}

TEST(lexer, ReadDecimalNumber) {
    auto stream = std::istringstream("3.14");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    double val;
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 3.14);
}

TEST(lexer, ReadScientificNotation) {
    auto stream = std::istringstream("2.998e8");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    double val;
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 2.998e8);
}

TEST(lexer, ReadUpperScientificNotation) {
    auto stream = std::istringstream("2.998E8");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    double val;
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 2.998e8);
}

TEST(lexer, IgnoreWhitespace) {
    auto stream = std::istringstream(" \n\r\t\v\f4");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    double val;
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 4);
}

TEST(lexer, ThrowOnBadInput) {
    // Unicode negative acknowledge
    auto stream = std::istringstream("\u0015");
    Lexer lexer(stream);
    EXPECT_THROW(lexer.read(), std::runtime_error);
}

TEST(lexer, ReadProgresses) {
    auto stream = std::istringstream("12 34");
    Lexer lexer(stream);

    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    double val;
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 12.0);

    tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 34.0);
}

TEST(lexer, PeekDoesNotProgress) {
    auto stream = std::istringstream("12 34");
    Lexer lexer(stream);

    Token tok = lexer.peek();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    double val;
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 12.0);

    tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 12.0);
}

void testSymbol(const char *input, Symbol expect) {
    auto stream = std::istringstream(input);
    Lexer lexer(stream);

    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::SYMBOL);
    Symbol val;
    ASSERT_NO_THROW(val = std::get<Symbol>(tok.data));
    EXPECT_EQ(val, expect);
}

TEST(lexer, SymbolAdd) {
   testSymbol("+", Symbol::ADD);
}

TEST(lexer, SymbolSub) {
   testSymbol("-", Symbol::SUBTRACT);
}

TEST(lexer, SymbolMul) {
   testSymbol("*", Symbol::MULTIPLY);
}

TEST(lexer, SymbolDiv) {
   testSymbol("/", Symbol::DIVIDE);
}

TEST(lexer, SymbolAnd) {
   testSymbol("&", Symbol::AND);
}

TEST(lexer, SymbolOr) {
   testSymbol("|", Symbol::OR);
}

TEST(lexer, SymbolXor) {
   testSymbol("^", Symbol::XOR);
}

TEST(lexer, SymbolNot) {
   testSymbol("~", Symbol::NOT);
}

TEST(lexer, SymbolOpenBracket) {
   testSymbol("(", Symbol::BRACKET_OPEN);
}

TEST(lexer, SymbolCloseBracket) {
   testSymbol(")", Symbol::BRACKET_CLOSE);
}

TEST(lexer, SemicolonEndsStatement) {
    auto stream = std::istringstream(";");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::END_OF_STMT);
    // Cast to void to disable nodiscard warning
    ASSERT_NO_THROW(static_cast<void>(std::get<std::monostate>(tok.data)));
}

TEST(lexer, ReadSymbolEmbedded) {
    auto stream = std::istringstream("1+2");
    Lexer lexer(stream);

    Token tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    double val;
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 1.0);

    tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::SYMBOL);
    Symbol sym;
    ASSERT_NO_THROW(sym = std::get<Symbol>(tok.data));
    EXPECT_EQ(sym, Symbol::ADD);

    tok = lexer.read();
    EXPECT_EQ(tok.type, TokenType::NUMBER);
    ASSERT_NO_THROW(val = std::get<double>(tok.data));
    EXPECT_DOUBLE_EQ(val, 2.0);
}

TEST(lexer, ReportsColumn) {
    auto stream = std::istringstream("+-");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.line, 1);
    EXPECT_EQ(tok.column, 1);

    tok = lexer.read();
    EXPECT_EQ(tok.line, 1);
    EXPECT_EQ(tok.column, 2);
}

TEST(lexer, ReportsLine) {
    auto stream = std::istringstream("+-\n+");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.line, 1);
    EXPECT_EQ(tok.column, 1);

    tok = lexer.read();
    EXPECT_EQ(tok.line, 1);
    EXPECT_EQ(tok.column, 2);
    
    tok = lexer.read();
    EXPECT_EQ(tok.line, 2);
    EXPECT_EQ(tok.column, 1);
}

TEST(lexer, DoesntProgressAfterEOS) {
    auto stream = std::istringstream("");
    Lexer lexer(stream);
    Token tok = lexer.read();
    EXPECT_EQ(tok.line, 1);
    EXPECT_EQ(tok.column, 1);

    tok = lexer.read();
    EXPECT_EQ(tok.line, 1);
    EXPECT_EQ(tok.column, 1);
}
