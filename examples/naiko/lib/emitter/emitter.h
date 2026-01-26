#pragma once

#include <aiko.h>

#include "../parser_nodes.h"

namespace aiko::naiko
{

    class Emitter
    {
    public:
        Emitter(const string file) : m_file(file) { }
        virtual ~Emitter() = default;
        virtual void emit(ProgramNode*) = 0;
    protected:
        const string m_file;
        virtual void emitNode(ASTNode*, size_t indent) = 0;
        virtual void save() = 0;
    };

}