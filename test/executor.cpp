#include <gtest/gtest.h>
#include <optional>
#include "executor.hpp"

using namespace quickcalc;

namespace {
    void testExecutor(StmtNode *stmt, std::optional<double> expected) {
        Executor executor;
        stmt->accept(executor);
        if (expected.has_value()) {
            EXPECT_TRUE(executor.hasResult());
            EXPECT_DOUBLE_EQ(executor.lastResult(), *expected);
        } else {
            EXPECT_FALSE(executor.hasResult());
        }
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

TEST(executor, CanDefineFunction) {
    auto stmt = std::make_unique<FuncDefNode>(
        "foo",
        std::make_unique<ConstNode>(1.0),
        std::vector<std::string>()
    );
    Executor executor;
    stmt->accept(executor);
    EXPECT_FALSE(executor.hasResult());

    const ExecutorState::Func *func = nullptr;
    EXPECT_TRUE(executor.getState().tryGetFunction("foo", func));
    EXPECT_NE(func, nullptr);
}

TEST(executor, CanInvokeFunction) {
    auto stmt1 = std::make_unique<FuncDefNode>(
        "foo",
        std::make_unique<ConstNode>(1.0),
        std::vector<std::string>()
    );
    auto stmt2 = std::make_unique<ExprStmtNode>(
            std::make_unique<FunctionInvocationNode>(
            "foo",
            std::vector<ExprNode::ptr>()
        )
    );
    Executor executor;
    stmt1->accept(executor);
    EXPECT_FALSE(executor.hasResult());

    stmt2->accept(executor);
    EXPECT_TRUE(executor.hasResult());
    EXPECT_DOUBLE_EQ(executor.lastResult(), 1.0);
}

TEST(executor, CanInvokeFunctionWithParam) {
    auto stmt1 = std::make_unique<FuncDefNode>(
        "foo",
        std::make_unique<BinaryOperationNode>(
            BinaryOperation::ADD,
            std::make_unique<ConstNode>(1.0),
            std::make_unique<FunctionInvocationNode>("a", std::vector<ExprNode::ptr>())
        ),
        std::vector<std::string>({ "a" })
    );
    
    std::vector<ExprNode::ptr> params;
    params.push_back(std::make_unique<ConstNode>(2.0));

    auto stmt2 = std::make_unique<ExprStmtNode>(
            std::make_unique<FunctionInvocationNode>(
            "foo",
            std::move(params)
        )
    );
    Executor executor;
    stmt1->accept(executor);
    EXPECT_FALSE(executor.hasResult());

    stmt2->accept(executor);
    EXPECT_TRUE(executor.hasResult());
    EXPECT_DOUBLE_EQ(executor.lastResult(), 3.0);
}
