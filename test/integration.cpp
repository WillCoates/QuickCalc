#include <gtest/gtest.h>
#include "lexer.hpp"
#include "parser.hpp"
#include "executor.hpp"
#include <sstream>

using namespace quickcalc;

class IntegrationTest: public testing::Test {
    std::vector<StmtNode::ptr> vitalNodes;
protected:
    std::istringstream input;
    Lexer lexer = Lexer(input);
    Parser parser = Parser(lexer);
    Executor executor;
    StmtNode::ptr &storeVital(StmtNode::ptr &&ast) {
        return ast->canSafeDelete() ? ast : vitalNodes.emplace_back(std::move(ast));
    }
};

TEST_F(IntegrationTest, Constant) {
    input.str("1");
    auto ast = parser.parse();
    auto &astRef = storeVital(std::move(ast));
    astRef->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), 1.0);
}

TEST_F(IntegrationTest, Addition) {
    input.str("1+2");
    auto ast = parser.parse();
    auto &astRef = storeVital(std::move(ast));
    astRef->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), 3.0);
}

TEST_F(IntegrationTest, Subtraction) {
    input.str("1-2");
    auto ast = parser.parse();
    auto &astRef = storeVital(std::move(ast));
    astRef->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), -1.0);
}

TEST_F(IntegrationTest, Multiplication) {
    input.str("1*2");
    auto ast = parser.parse();
    auto &astRef = storeVital(std::move(ast));
    astRef->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), 2.0);
}

TEST_F(IntegrationTest, Division) {
    input.str("1/2");
    auto ast = parser.parse();
    auto &astRef = storeVital(std::move(ast));
    astRef->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), 0.5);
}

TEST_F(IntegrationTest, FollowsBIDMAS) {
    input.str("3*(1+3-2/4)");
    auto ast = parser.parse();
    auto &astRef = storeVital(std::move(ast));
    astRef->accept(executor);
    EXPECT_DOUBLE_EQ(executor.lastResult(), 10.5);
}

TEST_F(IntegrationTest, MultiStatement) {
    input.str("1+2;3+4");
    auto ast = parser.parse();
    {
        auto &astRef = storeVital(std::move(ast));
        astRef->accept(executor);
    }
    EXPECT_DOUBLE_EQ(executor.lastResult(), 3.0);
    ast = parser.parse();
    {
        auto &astRef = storeVital(std::move(ast));
        astRef->accept(executor);
    }
    EXPECT_DOUBLE_EQ(executor.lastResult(), 7.0);
}

TEST_F(IntegrationTest, ExecuteFunction) {
    input.str("let one = 1; one");
    auto ast = parser.parse();
    {
        auto &astRef = storeVital(std::move(ast));
        astRef->accept(executor);
    }
    EXPECT_FALSE(executor.hasResult());
    ast = parser.parse();
    {
        auto &astRef = storeVital(std::move(ast));
        astRef->accept(executor);
    }
    EXPECT_TRUE(executor.hasResult());
    EXPECT_DOUBLE_EQ(executor.lastResult(), 1.0);
}

TEST_F(IntegrationTest, RecursiveFunction) {
    input.str("let a(x) = x; let b(x) = a(x); b(5)");
    for (int i = 0; i < 2; i++)
    {
        auto ast = parser.parse();
        auto &astRef = storeVital(std::move(ast));
        astRef->accept(executor);
        EXPECT_FALSE(executor.hasResult());
    }
    auto ast = parser.parse();
    {
        auto &astRef = storeVital(std::move(ast));
        astRef->accept(executor);
    }
    EXPECT_TRUE(executor.hasResult());
    EXPECT_DOUBLE_EQ(executor.lastResult(), 5.0);
}
