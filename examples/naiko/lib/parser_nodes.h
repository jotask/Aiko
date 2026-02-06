#pragma once

#include <aiko_types.h>

#include <vector>

#include "compiler_types.h"

namespace aiko::naiko
{

    struct ASTNode
    {
        NaikoType type = NaikoType::INVALID;
        virtual ~ASTNode() = default;
        virtual void print(size_t indent = 0) const = 0;
    };

    template<class T>
    using NodeUPtr = AikoUPtr<T>;

    using NodePtr = NodeUPtr<ASTNode>;

    // Values

    struct NumberNode : ASTNode
    {
        explicit NumberNode(int value) : value(value) { }
        const int value;
        virtual void print(size_t indent = 0) const override;
    };

    struct StringNode : ASTNode
    {
        explicit StringNode(string value) : value(value) { }
        string value;
        virtual void print(size_t indent = 0) const override;
    };

    // Operations

    struct BinaryOperationNode : ASTNode
    {
        explicit BinaryOperationNode(NodePtr l, NaikoOperation o, NodePtr r) : left(std::move(l)), operation(o), right(std::move(r)) {}
        NodePtr left;
        NaikoOperation operation;
        NodePtr right;
        virtual void print(size_t indent = 0) const override;
    };

    struct UnaryOperationNode : ASTNode
    {
        NaikoOperation operation;
        NodePtr operand;
        UnaryOperationNode(NaikoOperation o, NodePtr node) : operation(o), operand(std::move(node)) {}
        virtual void print(size_t indent = 0) const override;
    };

    // Keywords

    struct PrintNode : ASTNode
    {
        explicit PrintNode(NodePtr e) : expr(std::move(e)) { }
        NodePtr expr;
        virtual void print(size_t indent = 0) const override;
    };

    struct LetNode : ASTNode
    {
        explicit LetNode(NodePtr l, NodePtr r) : left(std::move(l)), right(std::move(r)) {}
        NodePtr left;
        NodePtr right;
        virtual void print(size_t indent = 0) const override;
    };

    struct SetNode : ASTNode
    {
        explicit SetNode(NodePtr l, NodePtr r) : left(std::move(l)), right(std::move(r)) {}
        NodePtr left;
        NodePtr right;
        virtual void print(size_t indent = 0) const override;
    };

    struct ArrayAccessNode : ASTNode
    {
        explicit ArrayAccessNode(string name, NodePtr idx) : name(name), index(std::move(idx)) {}
        string name;
        NodePtr index;
        virtual void print(size_t indent = 0) const override;
    };

    struct IfNode : ASTNode
    {
        explicit IfNode(NodePtr cond, std::vector<NodePtr> stmts) : condition(std::move(cond)), body(std::move(stmts)) {}
        NodePtr condition;
        std::vector<NodePtr> body;
        virtual void print(size_t indent = 0) const override;
    };

    struct WhileNode : ASTNode
    {
        explicit WhileNode(NodePtr cond, std::vector<NodePtr> stmts) : condition(std::move(cond)), body(std::move(stmts)) {}
        NodePtr condition;
        std::vector<NodePtr> body;
        virtual void print(size_t indent = 0) const override;
    };

    // Others

    struct VariableNode : ASTNode
    {
        explicit VariableNode(string n) : name(n) {}
        string name;
        virtual void print(size_t indent = 0) const override;
    };


    // Program

    struct ProgramNode : ASTNode
    {
        explicit ProgramNode() : statements() { }
        std::vector<NodePtr> statements;
        virtual void print(size_t indent = 0) const override;
    };

    struct EndNode : ASTNode
    {
        explicit EndNode(){ }
        virtual void print(size_t indent = 0) const override;
    };

}
