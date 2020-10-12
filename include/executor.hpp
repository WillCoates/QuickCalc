#pragma once
#include "ast.hpp"
#include <stack>
#include <unordered_map>
#include <string>
#include <functional>
#include <vector>

namespace quickcalc {
    class Executor;
    class ExecutorState;

    class ExecutorState {
    public:
        using Func = std::function<double(Executor &executor, const std::vector<ExprNode::ptr> &)>;
    private:
        std::unordered_map<std::string, Func> _funcMap;
        const ExecutorState *_parent;
    public:
        ExecutorState();
        ExecutorState(ExecutorState *parent);

        void setFunction(const std::string &name, const Func &function);
        void setFunction(const std::string &name, Func &&function);
        void setFunction(std::string &&name, Func &&function);

        const Func &getFunction(const std::string &name) const;
        bool hasFunction(const std::string &name) const;
        bool tryGetFunction(const std::string &name, const Func *&function) const;
    };

    class Executor: public NodeVisitor {
        std::stack<double> _valueStack;
        std::stack<ExecutorState> _stateStack;
        double _lastResult;
        bool _hasResult;
    public:
        Executor();
        Executor(NodeVisitor *next);

        void visit(ExprStmtNode *node) override;
        void visit(FuncDefNode *node) override;
        void visit(ConstNode *node) override;
        void visit(UnaryOperationNode *node) override;
        void visit(BinaryOperationNode *node) override;
        void visit(FunctionInvocationNode *node) override;

        double evaluate(ExprNode *node);

        double lastResult() const;
        bool hasResult() const;
    
        void push(double value);
        double pop();

        ExecutorState &getState();
        ExecutorState &pushState();
        void popState();
    };
}
