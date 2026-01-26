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
        void declare(const string, llvm::AllocaInst*);
        bool isDeclared(const string);
        llvm::AllocaInst* lookupVar(const string);
        std::vector<std::unordered_map<string, llvm::AllocaInst*>> m_scopeStack;
    };

}
