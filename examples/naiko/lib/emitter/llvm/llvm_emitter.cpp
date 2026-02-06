#include "llvm_emitter.h"

#include "compiler_helper.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>

#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

namespace aiko::naiko
{
    LlvmEmitter::LlvmEmitter(const string name)
        : Emitter(name)
        , m_context()
        , m_module(std::make_unique<llvm::Module>(name, m_context))
        , m_builder(m_context)
    {
    }

    LlvmEmitter::LlvmEmitter(const CompilerOptions opts)
        : Emitter(opts)
        , m_context()
        , m_module(std::make_unique<llvm::Module>(opts.outputFile.data(), m_context))
        , m_builder(m_context)
    {

        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        auto tripleStr = llvm::sys::getDefaultTargetTriple();
        llvm::Triple triple = llvm::Triple(tripleStr);

        std::string error;
        const llvm::Target* target = llvm::TargetRegistry::lookupTarget(tripleStr, error);

        if (target == nullptr)
        {
            llvm::errs() << error << "\n";
            std::exit(-1);
        }

        llvm::TargetOptions targetOpts;
        AikoUPtr<llvm::TargetMachine> TM(
            target->createTargetMachine(
                triple, "generic", "", targetOpts, std::optional<llvm::Reloc::Model>())
            );


        //  generate IR for this machine
        m_module->setTargetTriple(triple);
        m_module->setDataLayout(TM->createDataLayout());

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

        enterScope();
        for (AikoUPtr<ASTNode>& n : node->statements)
        {
            emitNode(n.get(), mainFnt);
        }
        exitScope();

        // return
        m_builder.CreateRet(m_builder.getInt32(EXIT_SUCCESS));

        // Verify function
        if (llvm::verifyFunction(*mainFnt, &llvm::errs()))
        {
            logger::Log::error("Function verification failed ::");
            std::exit(-1);
        }

        m_module->print(llvm::outs(), nullptr);

    }

    llvm::Value* LlvmEmitter::emitNode(ASTNode* node, llvm::Function* fnt)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        // END
        if (EndNode* const end = dynamic_cast<EndNode*>(node))
        {
            return nullptr;
        }

        // PRINT
        if (PrintNode* const print = dynamic_cast<PrintNode*>(node))
        {
            constexpr const char* fnt_printf = "printf";
            llvm::Function* printfFNT = m_module->getFunction(fnt_printf);
            if (printfFNT == nullptr)
            {
                llvm::FunctionType* printfType = llvm::FunctionType::get(
                    m_builder.getInt32Ty(), // printf returns int
                    llvm::PointerType::get(m_builder.getInt8Ty(), 0), // first argument arg*
                    true // varidic
                );

                printfFNT = llvm::Function::Create(
                    printfType,
                    llvm::Function::ExternalLinkage,
                    fnt_printf,
                    m_module.get()
                    );
            }

            // cal printf
            llvm::Value* valueToPrint = emitNode(print->expr.get(), fnt);

            switch (print->expr->type)
            {
            case NaikoType::STRING:
                {
                    llvm::Value* fmt = m_builder.CreateGlobalStringPtr("%s\n");
                    m_builder.CreateCall(printfFNT, {fmt, valueToPrint});
                }
                break;
            case NaikoType::INT:
                {
                    llvm::Value* fmt = m_builder.CreateGlobalStringPtr("%d\n");
                    m_builder.CreateCall(printfFNT, {fmt, valueToPrint});
                }
                break;
            default:
                {
                    logger::Log::error("Unsupported type int PRINT");
                    std::exit(-1);
                }
                break;
            }
            return nullptr;
        }

        // STRING
        if (StringNode* const str = dynamic_cast<StringNode*>(node))
        {
            return m_builder.CreateGlobalStringPtr(str->value);
        }

        // NUMBER
        if (NumberNode* const num = dynamic_cast<NumberNode*>(node))
        {
            return llvm::ConstantInt::get(m_context, llvm::APInt(32, num->value));
        }

        // LET
        if (LetNode* const let = dynamic_cast<LetNode*>(node))
        {
            // Check if this is an array declaration
            if (ArrayAccessNode* arr = dynamic_cast<ArrayAccessNode*>(let->left.get()))
            {
                // Allocate array with the given size
                const NumberNode* num = dynamic_cast<NumberNode*>(arr->index.get());
                if (num == nullptr)
                {
                    logger::Log::error("unvalid type on array");
                    std::exit(-1);
                }
                llvm::ArrayType* arrType = llvm::ArrayType::get(m_builder.getInt32Ty(), num->value);
                llvm::IRBuilder<> tmpBuilder(&fnt->getEntryBlock(), fnt->getEntryBlock().begin());
                llvm::AllocaInst* alloc = tmpBuilder.CreateAlloca(arrType, nullptr, arr->name);
                declare(arr->name, alloc);

                // No initialization yet
                return alloc;
            }
            if (VariableNode* var = dynamic_cast<VariableNode*>(let->left.get()))
            {
                llvm::IRBuilder<> tmpBuilder(&fnt->getEntryBlock(), fnt->getEntryBlock().begin());
                llvm::Value* value = emitNode(let->right.get(), fnt);
                llvm::Type* llvmType = value->getType();
                llvm::AllocaInst* alloc = tmpBuilder.CreateAlloca(llvmType, nullptr, var->name);
                declare(var->name, alloc);
                return m_builder.CreateStore(value, alloc);
            }
            AIKO_NOT_IMPLEMENTED;
        }

        // SET
        if (SetNode* const set = dynamic_cast<SetNode*>(node))
        {
            llvm::Value* target = getTargetPtr(set->left.get(), fnt, true);
            llvm::Value* value = emitNode(set->right.get(), fnt);
            return m_builder.CreateStore(value, target);
        }

        // Array
        if (ArrayAccessNode* const arr = dynamic_cast<ArrayAccessNode*>(node))
        {
            llvm::Value* target = getTargetPtr(arr, fnt, false);
            return m_builder.CreateLoad(m_builder.getInt32Ty(), target, arr->name + "_elem");
        }

        // IF
        if (IfNode* const ifN = dynamic_cast<IfNode*>(node))
        {
            llvm::Value* condition = emitNode(ifN->condition.get(), fnt);

            // Make sure condition is i1 (bool)
            if (condition-> getType()->isIntegerTy() == true)
            {
                condition = m_builder.CreateICmpNE(condition, llvm::ConstantInt::get(m_builder.getInt32Ty(), 0), "ifcond");
            }

            llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(m_context, "then", fnt);
            llvm::BasicBlock* elseBlock = nullptr;
            llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(m_context, "ifcondition", fnt);

            // TODO
            // if ( exist else Block)
            m_builder.CreateCondBr(condition, thenBlock, mergeBlock);

            // then
            m_builder.SetInsertPoint(thenBlock);
            enterScope();
            for (auto& stmt : ifN->body)
            {
                emitNode(stmt.get(), fnt);
            }
            exitScope();
            m_builder.CreateBr(mergeBlock);

            // else
            // TODO

            // merge block
            m_builder.SetInsertPoint(mergeBlock);

            return nullptr;
        }

        // WHILE
        if (WhileNode* const whileN = dynamic_cast<WhileNode*>(node))
        {

            llvm::BasicBlock* conditionBlock = llvm::BasicBlock::Create(m_context, "whilecondition", fnt);
            llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(m_context, "whilebody", fnt);
            llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(m_context, "whilecont", fnt);

            m_builder.CreateBr(conditionBlock);

            // Condition block
            m_builder.SetInsertPoint(conditionBlock);
            llvm::Value* condition = emitNode(whileN->condition.get(), fnt);
            if (condition->getType()->isIntegerTy(32))
            {
                condition = m_builder.CreateICmpNE(condition, llvm::ConstantInt::get(m_context, llvm::APInt(32, 0)), "whilecondition");
            }
            m_builder.CreateCondBr(condition, bodyBlock, mergeBlock);

            // body block
            m_builder.SetInsertPoint(bodyBlock);
            enterScope();
            for (auto& stmt : whileN->body)
            {
                emitNode(stmt.get(), fnt);
            }
            exitScope();

            // Jump to condition
            m_builder.CreateBr(conditionBlock);

            // continue after loop
            m_builder.SetInsertPoint(mergeBlock);

            return nullptr;


        }

        // BinaryOperationNode
        if (BinaryOperationNode* const bin = dynamic_cast<BinaryOperationNode*>(node))
        {
            llvm::Value* left = emitNode(bin->left.get(), fnt);
            llvm::Value* right = emitNode(bin->right.get(), fnt);

            switch (bin->operation)
            {
                // Arithmetic -> int
                case NaikoOperation::ADD:           return m_builder.CreateAdd(left, right, "add" );
                case NaikoOperation::SUBTRACT:      return m_builder.CreateSub(left, right, "sub" );
                case NaikoOperation::MULTIPLY:      return m_builder.CreateMul(left, right, "mult" );
                case NaikoOperation::DIVIDE:        return m_builder.CreateSDiv(left, right, "div" );
                case NaikoOperation::MODULO:        return m_builder.CreateSRem(left, right, "mod" );
                // Comparisons -> bool
                case NaikoOperation::GREATERTHAN:   return m_builder.CreateICmpSGT(left, right, "gt");
                case NaikoOperation::LESSTHAN:      return m_builder.CreateICmpSLT(left, right, "lt");
                case NaikoOperation::EQUAL:         return m_builder.CreateICmpEQ(left, right, "eq");
                default:
                AIKO_NOT_IMPLEMENTED;
            }
        }

        // UnaryOperationNode
        if (UnaryOperationNode* const un = dynamic_cast<UnaryOperationNode*>(node))
        {
            llvm::Value* unary = emitNode(un->operand.get(), fnt);
            switch (un->operation)
            {
                case NaikoOperation::SUBTRACT: return m_builder.CreateNeg(unary);
                default:
                    AIKO_NOT_IMPLEMENTED;
            }
        }

        // VariableNode
        if (VariableNode* const var = dynamic_cast<VariableNode*>(node))
        {
            llvm::AllocaInst* alloc = llvm::dyn_cast<llvm::AllocaInst>(getTargetPtr(var, fnt, false));
            if (!alloc)
            {
                logger::Log::error("Error variable with name [%s] not found in scope", var->name.c_str());
                std::exit(-1);
            }
            return m_builder.CreateLoad(alloc->getAllocatedType(), alloc, var->name + "_val");
        }

        AIKO_ASSERT(false, "NOT IMPLEMENTED")

    }

    llvm::Value* LlvmEmitter::getTargetPtr(ASTNode* node, llvm::Function* fnt, bool declareIfMissing)
    {
        if (VariableNode* var = dynamic_cast<VariableNode*>(node))
        {
            llvm::AllocaInst* alloc = lookupVar(var->name);
            if (alloc == nullptr)
            {
                if (declareIfMissing)
                {
                    logger::Log::error("Internal error: variable '%s' declared without LET", var->name.c_str());
                    std::exit(-1);
                }
                else
                {
                    logger::Log::error("Variable '%s' not declared", var->name.c_str());
                    std::exit(-1);
                }
            }
            return alloc;
        }
        if (ArrayAccessNode* arr = dynamic_cast<ArrayAccessNode*>(node))
        {
            llvm::AllocaInst* alloc = lookupVar(arr->name);
            if (alloc == nullptr)
            {
                if (declareIfMissing)
                {
                    // Allocate array with fixed size (example 100)
                    llvm::ArrayType* arrType = llvm::ArrayType::get(m_builder.getInt32Ty(), 100);
                    llvm::IRBuilder<> tmpBuilder(&fnt->getEntryBlock(), fnt->getEntryBlock().begin());
                    alloc = tmpBuilder.CreateAlloca(arrType, nullptr, arr->name);
                    declare(arr->name, alloc);
                }
                else
                {
                    logger::Log::error("Array '%s' not declared", arr->name.c_str());
                    std::exit(-1);
                }
            }

            llvm::Value* index = emitNode(arr->index.get(), fnt);
            return m_builder.CreateGEP(alloc->getAllocatedType(), alloc, {m_builder.getInt32(0), index});

        }
        AIKO_NOT_IMPLEMENTED;
    }

    void LlvmEmitter::save()
    {
        AIKO_NOT_IMPLEMENTED;
    }

    void LlvmEmitter::compile()
    {

        // 1. Generate LLVM definition program
        std::error_code error_code;
        const auto outFile = m_file + ".ll";
        llvm::raw_fd_stream out(outFile, error_code);
        if (error_code)
        {
            llvm::errs() << "could not open file" << error_code.message() << "\n";
            std::exit(error_code.value());
        }
        m_module->print(out, nullptr);
        out.flush();

        // Compile our .ll IR program to executable
        const string cmd = "clang " + outFile + " -o " + m_file;
        int result = std::system(cmd.c_str());
        if (result != EXIT_SUCCESS)
        {
            // On POSIX systems, exit code is in the high byte
            const int exitCode = WEXITSTATUS(result);
            logger::Log::info("Exit code: [%d]", exitCode);
            logger::Log::critical("Couldn't compile emitted code exited with error code [%d] -> [%s] -> [%s]", exitCode, m_file.c_str(), cmd.c_str());
        }
    }

    void LlvmEmitter::enterScope()
    {
        m_scopeStack.emplace_back();
    }

    void LlvmEmitter::exitScope()
    {
        m_scopeStack.pop_back();
    }

    void LlvmEmitter::declare(const string name, llvm::AllocaInst* fnt)
    {
        m_scopeStack.back().insert({name, fnt});
    }

    bool LlvmEmitter::isDeclared(const string name)
    {
        for (auto it = m_scopeStack.rbegin(); it != m_scopeStack.rend(); ++it)
        {
            if (it->contains(name) == true)
            {
                return true;
            }
        }
        return false;
    }

    llvm::AllocaInst* LlvmEmitter::lookupVar(const string name)
    {
        for (auto it = m_scopeStack.rbegin(); it != m_scopeStack.rend(); ++it)
        {
            if (it->contains(name) == true)
            {
                return it->at(name);
            }
        }
        return nullptr;
    }
}
