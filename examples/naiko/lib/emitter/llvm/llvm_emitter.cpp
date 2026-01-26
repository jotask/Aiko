#include "llvm_emitter.h"

#include "compiler_helper.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace aiko::naiko
{
    LlvmEmitter::LlvmEmitter(string file)
        : Emitter(file)
    {

    }

    void LlvmEmitter::emit(ProgramNode* node)
    {
        AIKO_NOT_IMPLEMENTED;
    }

    void LlvmEmitter::emitNode(ASTNode* node, size_t indent)
    {
        if (node == nullptr)
        {
            return;
        }

        // END
        if (EndNode* const end = dynamic_cast<EndNode*>(node))
        {
            return;
        }

        // PRINT
        if (PrintNode* const print = dynamic_cast<PrintNode*>(node))
        {
            AIKO_NOT_IMPLEMENTED;
            return;
        }

        // STRING
        if (StringNode* const str = dynamic_cast<StringNode*>(node))
        {
            AIKO_NOT_IMPLEMENTED;
            return;
        }

        // NUMBER
        if (NumberNode* const num = dynamic_cast<NumberNode*>(node))
        {
            AIKO_NOT_IMPLEMENTED;
            return;
        }

        // LET
        if (LetNode* const let = dynamic_cast<LetNode*>(node))
        {
            AIKO_NOT_IMPLEMENTED;
            return;
        }

        // IF
        if (IfNode* const ifN = dynamic_cast<IfNode*>(node))
        {
            AIKO_NOT_IMPLEMENTED;
            return;
        }

        // WHILE
        if (WhileNode* const whileN = dynamic_cast<WhileNode*>(node))
        {

            AIKO_NOT_IMPLEMENTED;
            return;
        }

        // BinaryOperationNode
        if (BinaryOperationNode* const bin = dynamic_cast<BinaryOperationNode*>(node))
        {

            AIKO_NOT_IMPLEMENTED;
            return;
        }

        // UnaryOperationNode
        if (UnaryOperationNode* const un = dynamic_cast<UnaryOperationNode*>(node))
        {
            AIKO_NOT_IMPLEMENTED;
            return;
        }

        // VariableNode
        if (VariableNode* const var = dynamic_cast<VariableNode*>(node))
        {
            AIKO_NOT_IMPLEMENTED;
            return;
        }

        AIKO_ASSERT(false, "NOT IMPLEMENTED")

    }

    void LlvmEmitter::save()
    {
        AIKO_NOT_IMPLEMENTED;
    }

    void LlvmEmitter::enterScope()
    {
        AIKO_NOT_IMPLEMENTED;
    }

    void LlvmEmitter::exitScope()
    {
        AIKO_NOT_IMPLEMENTED;
    }

    void LlvmEmitter::declare(const string name)
    {
        AIKO_NOT_IMPLEMENTED;
    }

    bool LlvmEmitter::isDeclared(const string name)
    {
        AIKO_NOT_IMPLEMENTED;
        return false;
    }

}
