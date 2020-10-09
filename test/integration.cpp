#include <gtest/gtest.h>
#include "lexer.hpp"
#include "parser.hpp"
#include "executor.hpp"
#include <sstream>

using namespace quickcalc;

class IntegrationTest: public testing::Test {
protected:
    std::istringstream input;
    Lexer lexer = Lexer(input);
    Parser parser = Parser(lexer);
    Executor executor;
};

TEST_F(IntegrationTest, Constant) {
    input.str("1");
    auto ast = parser.parse();
    ast->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), 1.0);
}

TEST_F(IntegrationTest, Addition) {
    input.str("1+2");
    auto ast = parser.parse();
    ast->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), 3.0);
}

TEST_F(IntegrationTest, Subtraction) {
    input.str("1-2");
    auto ast = parser.parse();
    ast->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), -1.0);
}

TEST_F(IntegrationTest, Multiplication) {
    input.str("1*2");
    auto ast = parser.parse();
    ast->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), 2.0);
}

TEST_F(IntegrationTest, Division) {
    input.str("1/2");
    auto ast = parser.parse();
    ast->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), 0.5);
}

TEST_F(IntegrationTest, FollowsBIDMAS) {
    input.str("3*(1+3-2/4)");
    auto ast = parser.parse();
    ast->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), 10.5);
}
