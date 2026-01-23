#include "parser_nodes.h"

#include <logger/logger.h>

#include "compiler_helper.h"

namespace aiko::naiko
{
    void NumberNode::print(size_t indent) const
    {
        logger::Log::info("%sNUMBER::[%d]", makeIndent(indent).c_str(), value);
    }

    void StringNode::print(size_t indent) const
    {
        logger::Log::info("%sSTRING::[%s]", makeIndent(indent).c_str(), value.data());
    }

    void BinaryOperationNode::print(size_t indent) const
    {
        logger::Log::info("%sBINARYOPERATION::[%s]", makeIndent(indent).c_str(), magic_enum::enum_name(operation).data());
        left->print(indent + 1);
        right->print(indent + 1);
    }

    void PrintNode::print(size_t indent) const
    {
        logger::Log::info("%sPRINT", makeIndent(indent).c_str());
        expr->print(indent + 1);
    }

    void LetNode::print(size_t indent) const
    {
        logger::Log::info("%sLET", makeIndent(indent).c_str());
        expr->print(indent + 1);
    }

    void ProgramNode::print(size_t indent) const
    {
        logger::Log::info("%sPROGRAM", makeIndent(indent).c_str());
        for (const auto& stmt : statements)
        {
            stmt->print(indent + 1);
        }
    }

    void EndNode::print(size_t indent) const
    {
        logger::Log::info("%sEND", makeIndent(indent).c_str());
    }

}
