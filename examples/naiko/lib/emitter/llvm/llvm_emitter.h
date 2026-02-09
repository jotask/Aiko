#pragma once

#include "emitter/emitter.h"

#include <vector>

namespace llvm
{
    class Value;
    class Function;
    class AllocaInst;
}

namespace aiko::naiko
{

    class LlvmEmitter : public Emitter
    {
    public:
        LlvmEmitter(const string name);
        LlvmEmitter(const CompilerOptions opts);
        virtual ~LlvmEmitter();
        virtual void emit(ProgramNode*) override;
    private:

        struct Impl;
        AikoUPtr<Impl> pimpl;

        virtual void save() override;
        virtual void compile() override;

        // Helper
        void emitFunction(FunctionNode*);
        llvm::Value* emitNode(ASTNode*, llvm::Function* fnt);

        llvm::Value* getTargetPtr(ASTNode* node, llvm::Function* fnt, bool declareIfMissing = false);
        // Scope stack
        void enterScope();
        void exitScope();
        void declare(const string, llvm::AllocaInst*);
        bool isDeclared(const string);
        llvm::AllocaInst* lookupVar(const string);
        std::vector<std::unordered_map<string, llvm::AllocaInst*>> m_scopeStack;
    };

}
