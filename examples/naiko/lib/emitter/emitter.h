#pragma once

#include <aiko.h>

#include "../parser_nodes.h"

namespace aiko::naiko
{

    class Emitter
    {
    public:
        Emitter(const string file);
        virtual ~Emitter() = default;
        virtual void emit(ProgramNode*) = 0;
    protected:
        virtual void emitNode(ASTNode*, size_t indent) = 0;
        void append(string text = "");
        void newLine(string line = "");
        void save();
        void clear();
    private:
        const string m_file;
        string m_code;
    };

}