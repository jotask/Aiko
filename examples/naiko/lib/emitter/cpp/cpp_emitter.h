#pragma once

#include "emitter/emitter.h"

namespace aiko::naiko
{

    class CppEmitter : public Emitter
    {
    public:
        CppEmitter(string file);
        virtual ~CppEmitter() = default;
        virtual void emit(ProgramNode*) override;
    private:
        virtual void emitNode(ASTNode*, size_t) override;
    };

}
