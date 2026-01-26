#pragma once

#include <aiko.h>

#include "parser_nodes.h"
#include "naiko_compiler_options.h"

namespace aiko::naiko
{

    class Emitter
    {
    public:
        Emitter(const string name) : m_file(name) { }
        Emitter(const CompilerOptions opts) : m_file(opts.outputFile) { }
        virtual ~Emitter() = default;
        virtual void emit(ProgramNode*) = 0;
        virtual void compile() = 0;
    protected:
        const string m_file;
        virtual void emitNode(ASTNode*, size_t indent) = 0;
        virtual void save() = 0;
    };

}