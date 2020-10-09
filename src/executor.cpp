#include "executor.hpp"
#include <cstdint>

using namespace quickcalc;

Executor::Executor(): NodeVisitor() {
}

Executor::Executor(NodeVisitor *next): NodeVisitor(next) {
}

void Executor::visit(ExprStmtNode *node) {
    node->expression()->accept(*this);
    _lastResult = pop();
}

void Executor::visit(ConstNode *node) {
    _valueStack.push(node->value());
}

void Executor::visit(UnaryOperationNode *node) {
    node->value()->accept(*this);
    double value = pop();
    switch (node->operation()) {
    case UnaryOperation::NEGATE:
        push(-value);
        break;
    case UnaryOperation::NOT:
        push(~static_cast<int32_t>(value));
        break;
    }
}

void Executor::visit(BinaryOperationNode *node) {
    node->lhs()->accept(*this);
    node->rhs()->accept(*this);
    double rhs = pop();
    double lhs = pop();
    double result;
    switch (node->operation()) {
    case BinaryOperation::ADD:
        result = lhs + rhs;
        break;
    case BinaryOperation::SUBTRACT:
        result = lhs - rhs;
        break;
    case BinaryOperation::MULTIPLY:
        result = lhs * rhs;
        break;
    case BinaryOperation::DIVIDE:
        result = lhs / rhs;
        break;
    case BinaryOperation::AND:
        result = static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs);
        break;
    case BinaryOperation::OR:
        result = static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs);
        break;
    case BinaryOperation::XOR:
        result = static_cast<int32_t>(lhs) ^ static_cast<int32_t>(rhs);
        break;
    }
    push(result);
}

double Executor::lastResult() {
    return _lastResult;
}

void Executor::push(double value) {
    _valueStack.push(value);
}

double Executor::pop() {
    double value = _valueStack.top();
    _valueStack.pop();
    return value;
}
