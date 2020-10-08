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
