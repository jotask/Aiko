#pragma once

#include "emitter/emitter.h"

#include <vector>
#include <unordered_set>

namespace aiko::naiko
{

    class CppEmitter : public Emitter
    {
    public:
        CppEmitter(const CompilerOptions opts);
        virtual ~CppEmitter() = default;
        virtual void emit(ProgramNode*) override;
    private:
        virtual void emitNode(ASTNode*, size_t) override;
        virtual void save() override;
        virtual void compile() override;

        string m_code;

        // Helpers
        void append(string text = "");
        void newLine(string line = "");

        // Scope stack
        void enterScope();
        void exitScope();
        void declare(const string name);
        bool isDeclared(const string name);
        std::vector<std::unordered_set<string>> m_scopeStack;

    };

}
