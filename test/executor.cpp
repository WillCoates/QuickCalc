#include <gtest/gtest.h>
#include "executor.hpp"

using namespace quickcalc;

namespace {
    void testExecutor(StmtNode *stmt, double expected) {
        Executor executor;
        stmt->accept(executor);
        EXPECT_DOUBLE_EQ(executor.lastResult(), expected);
    }
}

TEST(executor, ContstantReturnsConstant) {
    auto stmt = std::make_unique<ExprStmtNode>(
        std::make_unique<ConstNode>(1.0)
    );
    testExecutor(stmt.get(), 1.0);
}

TEST(executor, NegatationNegates) {
    auto stmt = std::make_unique<ExprStmtNode>(
        std::make_unique<UnaryOperationNode>(
            UnaryOperation::NEGATE,
            std::make_unique<ConstNode>(1.0)
        )
    );
    testExecutor(stmt.get(), -1.0);
}

TEST(executor, BitwiseNotNots) {
    auto stmt = std::make_unique<ExprStmtNode>(
        std::make_unique<UnaryOperationNode>(
            UnaryOperation::NOT,
            std::make_unique<ConstNode>(1.0)
        )
    );
    testExecutor(stmt.get(), ~1);
}

TEST(executor, AdditionAdds) {
    auto stmt = std::make_unique<ExprStmtNode>(
        std::make_unique<BinaryOperationNode>(
            BinaryOperation::ADD,
            std::make_unique<ConstNode>(1.0),
            std::make_unique<ConstNode>(2.0)
        )
    );
    testExecutor(stmt.get(), 3.0);
}

TEST(executor, SubtractionSubs) {
    auto stmt = std::make_unique<ExprStmtNode>(
        std::make_unique<BinaryOperationNode>(
            BinaryOperation::SUBTRACT,
            std::make_unique<ConstNode>(1.0),
            std::make_unique<ConstNode>(2.0)
        )
    );
    testExecutor(stmt.get(), -1.0);
}

TEST(executor, MultiplicationMuls) {
    auto stmt = std::make_unique<ExprStmtNode>(
        std::make_unique<BinaryOperationNode>(
            BinaryOperation::MULTIPLY,
            std::make_unique<ConstNode>(1.0),
            std::make_unique<ConstNode>(2.0)
        )
    );
    testExecutor(stmt.get(), 2.0);
}

TEST(executor, DivideDivs) {
    auto stmt = std::make_unique<ExprStmtNode>(
        std::make_unique<BinaryOperationNode>(
            BinaryOperation::DIVIDE,
            std::make_unique<ConstNode>(1.0),
            std::make_unique<ConstNode>(2.0)
        )
    );
    testExecutor(stmt.get(), 0.5);
}

TEST(executor, AndAnds) {
    auto stmt = std::make_unique<ExprStmtNode>(
        std::make_unique<BinaryOperationNode>(
            BinaryOperation::AND,
            std::make_unique<ConstNode>(1.0),
            std::make_unique<ConstNode>(3.0)
        )
    );
    testExecutor(stmt.get(), 1.0);
}

TEST(executor, OrOrs) {
    auto stmt = std::make_unique<ExprStmtNode>(
        std::make_unique<BinaryOperationNode>(
            BinaryOperation::OR,
            std::make_unique<ConstNode>(1.0),
            std::make_unique<ConstNode>(3.0)
        )
    );
    testExecutor(stmt.get(), 3.0);
}

TEST(executor, XorXors) {
    auto stmt = std::make_unique<ExprStmtNode>(
        std::make_unique<BinaryOperationNode>(
            BinaryOperation::XOR,
            std::make_unique<ConstNode>(1.0),
            std::make_unique<ConstNode>(3.0)
        )
    );
    testExecutor(stmt.get(), 2.0);
}
