#include "executor.hpp"
#include <cstdint>
#include <stdexcept>

using namespace quickcalc;

Executor::Executor(): NodeVisitor() {
    pushState();
}

Executor::Executor(NodeVisitor *next): NodeVisitor(next) {
    pushState();
}

void Executor::visit(ExprStmtNode *node) {
    _lastResult = evaluate(node->expression());
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

double Executor::evaluate(ExprNode *node) {
    node->accept(*this);
    return pop();
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

ExecutorState &Executor::getState() {
    return _stateStack.top();
}

ExecutorState &Executor::pushState() {
    if (_stateStack.empty()) {
        return _stateStack.emplace();
    } else {
        return _stateStack.emplace(&_stateStack.top());
    }
}

void Executor::popState() {
    _stateStack.pop();
}

ExecutorState::ExecutorState(): ExecutorState(nullptr) {
}

ExecutorState::ExecutorState(ExecutorState *parent): _parent(parent) {
}

void ExecutorState::setFunction(const std::string &name, const Func &function) {
    _funcMap[name] = function;
}

void ExecutorState::setFunction(const std::string &name, Func &&function) {
    _funcMap[name] = std::move(function);
}

void ExecutorState::setFunction(std::string &&name, Func &&function) {
    _funcMap[std::move(name)] = std::move(function);
}

const ExecutorState::Func &ExecutorState::getFunction(const std::string &name) const {
    const Func *func;
    if (tryGetFunction(name, func)) {
        return *func;
    } else {
        throw std::logic_error("Couldn't find function " + name);
    }
}

bool ExecutorState::hasFunction(const std::string &name) const {
    const Func *func;
    return tryGetFunction(name, func);
}

bool ExecutorState::tryGetFunction(const std::string &name, const Func *&function) const {
    auto it = _funcMap.find(name);
    if (it != _funcMap.end()) {
        function = &it->second;
        return true;
    } else if (_parent) {
        return _parent->tryGetFunction(name, function);
    } else {
        return false;
    }
}
