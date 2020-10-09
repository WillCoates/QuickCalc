#include "ast.hpp"

using namespace quickcalc;

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

NodeVisitor::NodeVisitor(): _next(nullptr) {
}

NodeVisitor::NodeVisitor(NodeVisitor *next): _next(next){
}

void NodeVisitor::visit(ExprStmtNode *node) {
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
