#pragma once

#include <aiko_types.h>

#include <vector>

#include "compiler_types.h"

namespace aiko::naiko
{

    struct ASTNode
    {
        virtual ~ASTNode() = default;
        virtual void print(size_t indent = 0) const = 0;
    };

    template<class T>
    using NodeUPtr = AikoUPtr<T>;

    using NodePtr = NodeUPtr<ASTNode>;

    struct NumberNode : ASTNode
    {
        explicit NumberNode(int value) : value(value) { }
        const int value;
        virtual void print(size_t indent) const override;
    };

    struct StringNode : ASTNode
    {
        explicit StringNode(string value) : value(value) { }
        string value;
        virtual void print(size_t indent) const override;
    };

    struct BinaryOperationNode : ASTNode
    {
        explicit BinaryOperationNode(NodePtr l, NaikoOperation o, NodePtr r) : left(std::move(l)), operation(o), right(std::move(r)) {}
        NodePtr left;
        NaikoOperation operation;
        NodePtr right;
        virtual void print(size_t indent) const override;
    };

    struct PrintNode : ASTNode
    {
        explicit PrintNode(NodePtr e) : expr(std::move(e)) { }
        NodePtr expr;
        virtual void print(size_t indent) const override;
    };

    struct LetNode : ASTNode
    {
        explicit LetNode(string sy, NodePtr e) : symbol(sy), expr(std::move(e)) {}
        string symbol;
        NodePtr expr;
        virtual void print(size_t indent) const override;
    };

    struct ProgramNode : ASTNode
    {
        explicit ProgramNode() : statements() { }
        std::vector<NodePtr> statements;
        virtual void print(size_t indent = 0) const override;
    };

    struct EndNode : ASTNode
    {
        explicit EndNode(){ }
        virtual void print(size_t indent) const override;
    };

}
