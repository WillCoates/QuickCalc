#include "ast.hpp"

using namespace quickcalc;

bool Node::operator!=(const Node &other) const {
    return !(*this == other);
}

ExprStmtNode::ExprStmtNode(ExprNode::ptr &&expression): _expression(std::move(expression)) {
}

ExprNode *ExprStmtNode::expression() const {
    return _expression.get();
}

void ExprStmtNode::accept(NodeVisitor &visitor) {
    visitor.visit(this);
}

bool ExprStmtNode::operator==(const Node &other) const {
    if (typeid(*this) != typeid(other)) {
        return false;
    }
    const ExprStmtNode &otherStmt = static_cast<const ExprStmtNode&>(other);
    return *_expression == *otherStmt._expression;
}

FuncDefNode::FuncDefNode(const std::string &name, ExprNode::ptr &&expression, std::vector<std::string> &&paramNames):
    _name(name), _expression(std::move(expression)), _paramNames(std::move(paramNames)) {
}

const std::string &FuncDefNode::name() const {
    return _name;
}

ExprNode *FuncDefNode::expression() const {
    return _expression.get();
}

const std::vector<std::string> &FuncDefNode::paramNames() const {
    return _paramNames;
}

void FuncDefNode::accept(NodeVisitor &visitor) {
    visitor.visit(this);
}

bool FuncDefNode::operator==(const Node &other) const {
    if (typeid(*this) != typeid(other)) {
        return false;
    }
    const FuncDefNode &otherStmt = static_cast<const FuncDefNode&>(other);
    return _name == otherStmt._name
           && _paramNames == otherStmt._paramNames
           && *_expression == *otherStmt._expression;
}

ConstNode::ConstNode(double value): _value(value) {
}

double ConstNode::value() const {
    return _value;
}

void ConstNode::accept(NodeVisitor &visitor) {
    visitor.visit(this);
}

bool ConstNode::operator==(const Node &other) const {
    if (typeid(*this) != typeid(other)) {
        return false;
    }
    const ConstNode &otherExpr = static_cast<const ConstNode&>(other);
    return _value == otherExpr._value;
}

UnaryOperationNode::UnaryOperationNode(UnaryOperation operation, ExprNode::ptr &&value): _operation(operation), _value(std::move(value)) {
}

UnaryOperation UnaryOperationNode::operation() const {
    return _operation;
}

ExprNode *UnaryOperationNode::value() const {
    return _value.get();
}

void UnaryOperationNode::accept(NodeVisitor &visitor) {
    visitor.visit(this);
}

bool UnaryOperationNode::operator==(const Node &other) const {
    if (typeid(*this) != typeid(other)) {
        return false;
    }
    const UnaryOperationNode &otherExpr = static_cast<const UnaryOperationNode&>(other);
    return _operation == otherExpr._operation
           && *_value == *otherExpr._value;
}

BinaryOperationNode::BinaryOperationNode(BinaryOperation operation, ExprNode::ptr &&lhs, ExprNode::ptr &&rhs): _operation(operation), _lhs(std::move(lhs)), _rhs(std::move(rhs)) {
}

BinaryOperation BinaryOperationNode::operation() const {
    return _operation;
}

ExprNode *BinaryOperationNode::lhs() const {
    return _lhs.get();
}

ExprNode *BinaryOperationNode::rhs() const {
    return _rhs.get();
}

void BinaryOperationNode::accept(NodeVisitor &visitor) {
    visitor.visit(this);
}

bool BinaryOperationNode::operator==(const Node &other) const {
    if (typeid(*this) != typeid(other)) {
        return false;
    }
    const BinaryOperationNode &otherExpr = static_cast<const BinaryOperationNode&>(other);
    return _operation == otherExpr._operation
           && *_lhs == *otherExpr._lhs
           && *_rhs == *otherExpr._rhs;
}

FunctionInvocationNode::FunctionInvocationNode(const std::string &name, std::vector<std::unique_ptr<ExprNode>> &&params):
    _name(name), _params(std::move(params)) {
}

const std::string &FunctionInvocationNode::name() const {
    return _name;
}

const std::vector<std::unique_ptr<ExprNode>> &FunctionInvocationNode::params() const {
    return _params;
}

void FunctionInvocationNode::accept(NodeVisitor &visitor) {
    visitor.visit(this);
}

bool FunctionInvocationNode::operator==(const Node &other) const {
    if (typeid(*this) != typeid(other)) {
        return false;
    }
    const FunctionInvocationNode &otherExpr = static_cast<const FunctionInvocationNode&>(other);
    if (_name != otherExpr._name || _params.size() != otherExpr._params.size()) {
        return false;
    }
    for (int i = 0; i < _params.size(); i++ ) {
        if (*_params[i] != *otherExpr._params[i]) {
            return false;
        }
    }
    return true;
}

NodeVisitor::NodeVisitor(): _next(nullptr) {
}

NodeVisitor::NodeVisitor(NodeVisitor *next): _next(next){
}

void NodeVisitor::visit(ExprStmtNode *node) {
    if (_next) {
        _next->visit(node);
    }
}

void NodeVisitor::visit(FuncDefNode *node) {
    if (_next) {
        _next->visit(node);
    }
}

void NodeVisitor::visit(ConstNode *node) {
    if (_next) {
        _next->visit(node);
    }
}

void NodeVisitor::visit(UnaryOperationNode *node) {
    if (_next) {
        _next->visit(node);
    }
}

void NodeVisitor::visit(BinaryOperationNode *node) {
    if (_next) {
        _next->visit(node);
    }
}

void NodeVisitor::visit(FunctionInvocationNode *node) {
    if (_next) {
        _next->visit(node);
    }
}
