#include <gtest/gtest.h>
#include "executor.hpp"

using namespace quickcalc;

TEST(executorstate, UndefinedFunctionDoesNotExist) {
    ExecutorState state = ExecutorState();
    const ExecutorState::Func *func;
    EXPECT_FALSE(state.tryGetFunction("foo", func));
    EXPECT_FALSE(state.hasFunction("foo"));
}

TEST(executorstate, CanStoreFunction) {
    ExecutorState state = ExecutorState();
    const ExecutorState::Func *func;
    state.setFunction("foo", [] (auto&, const auto&) { return 0.0; });
    EXPECT_TRUE(state.tryGetFunction("foo", func));
    EXPECT_TRUE(state.hasFunction("foo"));
    EXPECT_NE(func, nullptr);
}

TEST(executorstate, CanRetreiveFunctionFromParent) {
    ExecutorState parent = ExecutorState();
    ExecutorState child = ExecutorState(&parent);
    const ExecutorState::Func *func;
    parent.setFunction("foo", [] (auto&, const auto&) { return 0.0; });
    EXPECT_TRUE(child.tryGetFunction("foo", func));
    EXPECT_TRUE(child.hasFunction("foo"));
    EXPECT_NE(func, nullptr);
}
