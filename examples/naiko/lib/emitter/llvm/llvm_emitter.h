#pragma once

#include "emitter/emitter.h"

#include <vector>
#include <unordered_set>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace aiko::naiko
{

    class LlvmEmitter : public Emitter
    {
    public:
        LlvmEmitter(const string name);
        LlvmEmitter(const CompilerOptions opts);
        virtual ~LlvmEmitter() = default;
        virtual void emit(ProgramNode*) override;
    private:

        llvm::LLVMContext m_context;
        AikoUPtr<llvm::Module> m_module;
        llvm::IRBuilder<> m_builder;

        virtual void save() override;
        virtual void compile() override;

        // Helper
        llvm::Value* emitNode(ASTNode*, llvm::Function* fnt);

        // Scope stack
        void enterScope();
        void exitScope();
        void declare(const string name);
        bool isDeclared(const string name);
        std::vector<std::unordered_set<string>> m_scopeStack;
    };

}
