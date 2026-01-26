#pragma once

#include "emitter/emitter.h"

#include <vector>
#include <unordered_set>

namespace aiko::naiko
{

    class LlvmEmitter : public Emitter
    {
    public:
        LlvmEmitter(string file);
        virtual ~LlvmEmitter() = default;
        virtual void emit(ProgramNode*) override;
    private:
        virtual void emitNode(ASTNode*, size_t) override;
        virtual void save() override;

        // Scope stack
        void enterScope();
        void exitScope();
        void declare(const string name);
        bool isDeclared(const string name);
        std::vector<std::unordered_set<string>> m_scopeStack;
    };

}
