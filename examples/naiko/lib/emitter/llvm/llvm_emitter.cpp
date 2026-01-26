#include "llvm_emitter.h"

#include "compiler_helper.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

namespace aiko::naiko
{
    LlvmEmitter::LlvmEmitter(const CompilerOptions opts)
        : Emitter(opts)
        , m_context()
        , m_module(std::make_unique<llvm::Module>(opts.outputFile.data(), m_context))
        , m_builder(m_context)
    {

    }

    void LlvmEmitter::emit(ProgramNode* node)
    {
        // Function type : int main()
        llvm::FunctionType* functType = llvm::FunctionType::get(m_builder.getInt32Ty(), false);

        // Create function in module
        llvm::Function* mainFnt = llvm::Function::Create(functType, llvm::Function::ExternalLinkage, "main", m_module.get());

        // Create entry basic block
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(m_context, "entry", mainFnt);

        m_builder.SetInsertPoint(entry);

        // return
        m_builder.CreateRet(m_builder.getInt32(EXIT_SUCCESS));

        // Verify function
        if (llvm::verifyFunction(*mainFnt, &llvm::errs()))
        {
            logger::Log::error("Function verification failed ::");
            std::exit(-1);
        }

        //  print

        m_module->print(llvm::outs(), nullptr);

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
