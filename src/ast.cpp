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

ConstNode::ConstNode(double value): _value(value) {
}

double ConstNode::value() const {
    return _value;
}

void ConstNode::accept(NodeVisitor &visitor) {
    visitor.visit(this);
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
