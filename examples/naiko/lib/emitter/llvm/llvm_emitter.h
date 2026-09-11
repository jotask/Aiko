#pragma once

#include "emitter/emitter.h"

#include <vector>

namespace llvm
{
    class Value;
    class Type;
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
        void emitGlobalVariable(ASTNode* node);
        void emitFunction(FunctionNode*);
        llvm::Value* emitNode(ASTNode*, llvm::Function* fnt);

        llvm::Value* getTargetPtr(ASTNode* node, llvm::Function* fnt, bool declareIfMissing = false);
        llvm::Type* toLLVMType(NaikoType);
        // Scope stack
        struct LlvmSymbol
        {
            llvm::Value* value;
            bool isConstant;
        };
        void enterScope();
        void exitScope();
        void declare(const string, llvm::Value*, bool = false);
        bool isDeclared(const string);
        LlvmSymbol* lookupVar(const string);
        std::vector<std::unordered_map<string, LlvmSymbol>> m_scopeStack;
    };

}
