#pragma once
#include <memory>

namespace quickcalc {
    class Node;
    class NodeVisitor;

    class Node {
    public:
        virtual void accept(NodeVisitor &visitor) = 0;
        virtual bool operator==(const Node &other) const = 0;
    };

    class StmtNode: public Node {
    public:
        using ptr = std::unique_ptr<StmtNode>;
    };

    class ExprNode: public Node {
    public:
        using ptr = std::unique_ptr<ExprNode>;
    };

    class ExprStmtNode: public StmtNode {
        ExprNode::ptr _expression;
    public:
        ExprStmtNode(ExprNode::ptr &&expression);
        ExprNode *expression() const;

        void accept(NodeVisitor &visitor) override;
        bool operator==(const Node &other) const override;
    };

    class ConstNode: public ExprNode {
        double _value;
    public:
        ConstNode(double value);
        double value() const;

        void accept(NodeVisitor &visitor) override;
        bool operator==(const Node &other) const override;
    };
    
    enum class UnaryOperation: int {
        NOT = 0,
        NEGATE,
    };

    class UnaryOperationNode: public ExprNode {
        UnaryOperation _operation;
        ExprNode::ptr _value;
    public:
        UnaryOperationNode(UnaryOperation operation, ExprNode::ptr &&value);
        UnaryOperation operation() const;
        ExprNode *value() const;

        void accept(NodeVisitor &visitor) override;
        bool operator==(const Node &other) const override;
    };

    enum class BinaryOperation: int {
        ADD = 0,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        AND,
        OR,
        XOR,
    };

    class BinaryOperationNode: public ExprNode {
        BinaryOperation _operation;
        ExprNode::ptr _lhs, _rhs;
    public:
        BinaryOperationNode(BinaryOperation operation, ExprNode::ptr &&lhs, ExprNode::ptr &&rhs);
        BinaryOperation operation() const;
        ExprNode *lhs() const;
        ExprNode *rhs() const;

        void accept(NodeVisitor &visitor) override;
        bool operator==(const Node &other) const override;
    };

    class NodeVisitor {
        NodeVisitor *_next;
    public:
        virtual ~NodeVisitor() = default;
        virtual void visit(ExprStmtNode *node);
        virtual void visit(ConstNode *node);
        virtual void visit(UnaryOperationNode *node);
        virtual void visit(BinaryOperationNode *node);
    protected:
        NodeVisitor();
        NodeVisitor(NodeVisitor *next);
    };
}
