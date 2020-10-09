#pragma once
#include "ast.hpp"
#include <stack>

namespace quickcalc {
    class Executor: public NodeVisitor {
        std::stack<double> _valueStack;
        double _lastResult;
    public:
        Executor();
        Executor(NodeVisitor *next);
        void visit(ExprStmtNode *node) override;
        void visit(ConstNode *node) override;
        void visit(UnaryOperationNode *node) override;
        void visit(BinaryOperationNode *node) override;

        double lastResult();
    private:
        void push(double value);
        double pop();
    };
}
