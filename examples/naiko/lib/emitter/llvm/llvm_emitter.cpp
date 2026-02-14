#include "llvm_emitter.h"

#include "compiler_helper.h"

// HEADER

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

// CPP
#include <llvm/CodeGen/RDFGraph.h>
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

    struct LlvmEmitter::Impl
    {
        llvm::LLVMContext m_context;
        AikoUPtr<llvm::Module> m_module;
        llvm::IRBuilder<> m_builder;

        Impl(const string name)
            : m_context()
            , m_module(std::make_unique<llvm::Module>(name, m_context))
            , m_builder(m_context)
        {

        }
    };

    LlvmEmitter::LlvmEmitter(const string name)
        : Emitter(name)
        , pimpl(std::make_unique<Impl>(name))
    {

    }

    LlvmEmitter::LlvmEmitter(const CompilerOptions opts)
        : Emitter(opts)
        , pimpl(std::make_unique<Impl>(opts.outputFile.data()))
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
        pimpl->m_module->setTargetTriple(triple);
        pimpl->m_module->setDataLayout(TM->createDataLayout());

    }

    LlvmEmitter::~LlvmEmitter() = default;

    void LlvmEmitter::emit(ProgramNode* node)
    {

        enterScope();

        // Emit global variables
        for (auto& stmt : node->statements)
        {
            if (LetNode* let = dynamic_cast<LetNode*>(stmt.get()))
            {
                if (let->constant == true)
                {
                    emitGlobalVariable(let);
                }
            }
        }

        // Emit all function declarations
        for (auto& stmt : node->statements)
        {
            if (FunctionNode* fnt = dynamic_cast<FunctionNode*>(stmt.get()))
            {
                emitFunction(fnt);
            }
        }

        // emit main

        // Function type : int main()
        llvm::FunctionType* functType = llvm::FunctionType::get(pimpl->m_builder.getInt32Ty(), false);

        // Create function in module
        llvm::Function* mainFnt = llvm::Function::Create(functType, llvm::Function::ExternalLinkage, "main", pimpl->m_module.get());

        // Create entry basic block
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(pimpl->m_context, "entry", mainFnt);

        pimpl->m_builder.SetInsertPoint(entry);

        enterScope();
        for (NodePtr& n : node->statements)
        {
            if (!dynamic_cast<FunctionNode*>(n.get()))
            {
                emitNode(n.get(), mainFnt);
            }
        }
        exitScope();

        // return

        if (pimpl->m_builder.GetInsertBlock()->getTerminator() == nullptr)
        {
            pimpl->m_builder.CreateRet(pimpl->m_builder.getInt32(EXIT_SUCCESS));
        }

        llvm::errs() << *pimpl->m_module; // prints entire IR to stderr

        if (llvm::verifyModule(*pimpl->m_module, &llvm::errs()))
        {
            logger::Log::error("Module verification failed");
            std::exit(-1);
        }

        pimpl->m_module->print(llvm::outs(), nullptr);

    }

    llvm::Value* LlvmEmitter::emitNode(ASTNode* node, llvm::Function* fnt)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        if (fnt == nullptr)
        {
            logger::Log::error("Internal compiler error: emitNode called without function context");
            std::exit(-1);
        }

        AIKO_ASSERT(!dynamic_cast<FunctionNode*>(node),"emitNode must never receive FunctionNode");

        // END
        if (EndNode* const end = dynamic_cast<EndNode*>(node))
        {
            return nullptr;
        }

        // PRINT
        if (PrintNode* const print = dynamic_cast<PrintNode*>(node))
        {
            constexpr const char* fnt_printf = "printf";
            llvm::Function* printfFNT = pimpl->m_module->getFunction(fnt_printf);
            if (printfFNT == nullptr)
            {
                llvm::FunctionType* printfType = llvm::FunctionType::get(
                    pimpl->m_builder.getInt32Ty(), // printf returns int
                    llvm::PointerType::get(pimpl->m_builder.getInt8Ty()->getContext(), 0), // first argument arg*
                    true // varidic
                );

                printfFNT = llvm::Function::Create(
                    printfType,
                    llvm::Function::ExternalLinkage,
                    fnt_printf,
                    pimpl->m_module.get()
                    );
            }

            // cal printf
            llvm::Value* valueToPrint = emitNode(print->expr.get(), fnt);

            switch (print->expr->type)
            {
            case NaikoType::STRING:
                {
                    llvm::Value* fmtGV = pimpl->m_builder.CreateGlobalString("%s\n");
                    llvm::Value* fmt = pimpl->m_builder.CreateBitCast(fmtGV,llvm::PointerType::get(pimpl->m_builder.getInt8Ty()->getContext(), 0));
                    pimpl->m_builder.CreateCall(printfFNT, {fmt, valueToPrint});
                }
                break;
            case NaikoType::INT:
                {
                    llvm::Value* fmtGV = pimpl->m_builder.CreateGlobalString("%d\n");
                    llvm::Value* fmt = pimpl->m_builder.CreateBitCast(fmtGV,llvm::PointerType::get(pimpl->m_builder.getInt8Ty()->getContext(), 0));
                    pimpl->m_builder.CreateCall(printfFNT, {fmt, valueToPrint});
                }
                break;
            case NaikoType::CHAR:
                {
                    llvm::Value* fmtGV = pimpl->m_builder.CreateGlobalString("%c\n");
                    llvm::Value* fmt = pimpl->m_builder.CreateBitCast(fmtGV,llvm::PointerType::get(pimpl->m_builder.getInt8Ty()->getContext(), 0));
                    // promote char (i8) to i32 printf
                    llvm::Value* promoted = pimpl->m_builder.CreateSExt(valueToPrint, pimpl->m_builder.getInt32Ty());
                    pimpl->m_builder.CreateCall(printfFNT, {fmt, promoted});
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

        // CHAR
        if (CharNode* const ch = dynamic_cast<CharNode*>(node))
        {
           return llvm::ConstantInt::get(pimpl->m_builder.getInt8Ty(), static_cast<uint64_t>(ch->value));
        }

        // STRING
        if (StringNode* const str = dynamic_cast<StringNode*>(node))
        {
            llvm::Value* strGV = pimpl->m_builder.CreateGlobalString(str->value);
            llvm::Value* strPtr = pimpl->m_builder.CreateBitCast(
                strGV,
                llvm::PointerType::get(pimpl->m_builder.getInt8Ty()->getContext(), 0)
            );
            return strPtr;
        }

        // NUMBER
        if (NumberNode* const num = dynamic_cast<NumberNode*>(node))
        {
            return llvm::ConstantInt::get(pimpl->m_context, llvm::APInt(32, num->value));
        }

        // LET
        if (LetNode* const let = dynamic_cast<LetNode*>(node))
        {
            if (ArrayAccessNode* arr = dynamic_cast<ArrayAccessNode*>(let->left.get()))
            {
                // get the array variable name
                VariableNode* var = dynamic_cast<VariableNode*>(arr->base.get());
                if (var == nullptr)
                {
                    logger::Log::error("ArrayAccessNode must have VariableNode as array");
                    std::exit(-1);
                }

                // Allocate array
                const NumberNode* sizeNode = dynamic_cast<NumberNode*>(arr->index.get());
                if (sizeNode == nullptr)
                {
                    logger::Log::error("Array size must be a number literal");
                    std::exit(-1);
                }

                llvm::ArrayType* arrType = llvm::ArrayType::get(pimpl->m_builder.getInt32Ty(), sizeNode->value);
                llvm::IRBuilder<> tmpBuilder(&fnt->getEntryBlock(), fnt->getEntryBlock().begin());
                llvm::AllocaInst* alloc = tmpBuilder.CreateAlloca(arrType, nullptr, var->name);
                declare(var->name, alloc);

                return alloc;
            }
            if (VariableNode* var = dynamic_cast<VariableNode*>(let->left.get()))
            {
                llvm::IRBuilder<> tmpBuilder(&fnt->getEntryBlock(), fnt->getEntryBlock().begin());
                llvm::Value* value = emitNode(let->right.get(), fnt);
                llvm::Type* llvmType = value->getType();
                llvm::AllocaInst* alloc = tmpBuilder.CreateAlloca(llvmType, nullptr, var->name);
                declare(var->name, alloc);
                return pimpl->m_builder.CreateStore(value, alloc);
            }
            AIKO_NOT_IMPLEMENTED;
        }

        // SET
        if (SetNode* const set = dynamic_cast<SetNode*>(node))
        {
            if (auto var = dynamic_cast<VariableNode*>(set->left.get()))
            {
                auto* sym = lookupVar(var->name);
                if (sym->isConstant == true)
                {
                    logger::Log::error("Cannot assign to constant variable '%s'", var->name.c_str());
                    std::exit(-1);
                }
            }
            llvm::Value* target = getTargetPtr(set->left.get(), fnt, true);
            llvm::Value* value = emitNode(set->right.get(), fnt);
            return pimpl->m_builder.CreateStore(value, target);
        }

        // Array
        if (ArrayAccessNode* const arr = dynamic_cast<ArrayAccessNode*>(node))
        {
            llvm::Value* elemPtr = getTargetPtr(arr, fnt, false);
            llvm::Type* prtType = elemPtr->getType();

            if (prtType->isPointerTy() == false)
            {
                llvm::errs() << "Array access did not return pointer";
                std::exit(-1);
            }

            if (VariableNode* var = dynamic_cast<VariableNode*>(arr->base.get()))
            {
                auto* sym = lookupVar(var->name);
                llvm::AllocaInst* alloc = llvm::dyn_cast<llvm::AllocaInst>(sym->value);

                llvm::Type* allocatedType = alloc->getAllocatedType();

                if (allocatedType->isArrayTy() == true)
                {
                    // return int
                    return pimpl->m_builder.CreateLoad(pimpl->m_builder.getInt32Ty(), elemPtr);
                }

                if (allocatedType->isPointerTy() == true)
                {
                    // string i8*
                    return pimpl->m_builder.CreateLoad(pimpl->m_builder.getInt8Ty(), elemPtr);
                }
            }

            llvm::errs() << "Unknow array acess type \n";
            std::exit(-1);

        }

        // IF
        if (IfNode* const ifN = dynamic_cast<IfNode*>(node))
        {
            llvm::Value* condition = emitNode(ifN->condition.get(), fnt);

            if (condition->getType()->isIntegerTy(1) == false)
            {
                // Make sure condition is i1 (bool)
                if (condition-> getType()->isIntegerTy() == true)
                {
                    condition = pimpl->m_builder.CreateICmpNE(condition, llvm::ConstantInt::get(pimpl->m_builder.getInt32Ty(), 0), "ifcond");
                }
            }

            llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(pimpl->m_context, "then", fnt);
            llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(pimpl->m_context, "else", fnt);
            llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(pimpl->m_context, "ifcondition", fnt);

            // Conditional branch
            pimpl->m_builder.CreateCondBr(condition, thenBlock, elseBlock);

            // THEN
            pimpl->m_builder.SetInsertPoint(thenBlock);
            enterScope();
            for (auto& stmt : ifN->thenBody)
            {
                emitNode(stmt.get(), fnt);
            }
            exitScope();
            if (pimpl->m_builder.GetInsertBlock()->getTerminator() == nullptr)
            {
                pimpl->m_builder.CreateBr(mergeBlock);
            }

            // else
            pimpl->m_builder.SetInsertPoint(elseBlock);
            enterScope();
            for (auto& stmt : ifN->elseBody)
            {
                emitNode(stmt.get(), fnt);
            }
            exitScope();
            if (pimpl->m_builder.GetInsertBlock()->getTerminator() == nullptr)
            {
                pimpl->m_builder.CreateBr(mergeBlock);
            }

            // merge block
            pimpl->m_builder.SetInsertPoint(mergeBlock);

            llvm::BasicBlock* current = pimpl->m_builder.GetInsertBlock();
            if (current->getTerminator() == nullptr)
            {
                llvm::BasicBlock* next = llvm::BasicBlock::Create(
                    pimpl->m_context,
                    "after_if",
                    fnt
                );
                pimpl->m_builder.CreateBr(next);
                pimpl->m_builder.SetInsertPoint(next);
            }

            return nullptr;
        }

        // WHILE
        if (WhileNode* const whileN = dynamic_cast<WhileNode*>(node))
        {

            llvm::BasicBlock* conditionBlock = llvm::BasicBlock::Create(pimpl->m_context, "whilecondition", fnt);
            llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(pimpl->m_context, "whilebody", fnt);
            llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(pimpl->m_context, "whilecont", fnt);

            pimpl->m_builder.CreateBr(conditionBlock);

            // Condition block
            pimpl->m_builder.SetInsertPoint(conditionBlock);
            llvm::Value* condition = emitNode(whileN->condition.get(), fnt);
            if (condition->getType()->isIntegerTy(1) == false)
            {
                if (condition->getType()->isIntegerTy(32) == true)
                {
                    condition = pimpl->m_builder.CreateICmpNE(
                        condition,
                        llvm::ConstantInt::get(pimpl->m_context, llvm::APInt(32, 0)),
                        "whilecond"
                    );
                }
            }
            pimpl->m_builder.CreateCondBr(condition, bodyBlock, mergeBlock);

            // body block
            pimpl->m_builder.SetInsertPoint(bodyBlock);
            enterScope();
            for (auto& stmt : whileN->body)
            {
                emitNode(stmt.get(), fnt);
            }
            exitScope();

            if (pimpl->m_builder.GetInsertBlock()->getTerminator() == nullptr)
            {
                pimpl->m_builder.CreateBr(conditionBlock);
            }

            // continue after loop
            pimpl->m_builder.SetInsertPoint(mergeBlock);

            return nullptr;

        }

        // BinaryOperationNode
        if (BinaryOperationNode* const bin = dynamic_cast<BinaryOperationNode*>(node))
        {
            llvm::Value* left = emitNode(bin->left.get(), fnt);
            llvm::Value* right = emitNode(bin->right.get(), fnt);

            // Promote integers to same width
            if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy())
            {
                unsigned leftBits  = left->getType()->getIntegerBitWidth();
                unsigned rightBits = right->getType()->getIntegerBitWidth();

                if (leftBits < rightBits)
                {
                    left = pimpl->m_builder.CreateSExt(left, right->getType());
                }
                else if (rightBits < leftBits)
                {
                    right = pimpl->m_builder.CreateSExt(right, left->getType());
                }

            }

            if (left->getType() != right->getType())
            {
                left->getType()->print(llvm::errs());
                llvm::errs() << "\n";
                right->getType()->print(llvm::errs());
                llvm::errs() << "\n";

                logger::Log::error("Type mismatch in comparison");
                std::exit(-1);
            }

            switch (bin->operation)
            {
                // Arithmetic -> int
                case NaikoOperation::ADD:               return pimpl->m_builder.CreateAdd(left, right, "add" );
                case NaikoOperation::SUBTRACT:          return pimpl->m_builder.CreateSub(left, right, "sub" );
                case NaikoOperation::MULTIPLY:          return pimpl->m_builder.CreateMul(left, right, "mult" );
                case NaikoOperation::DIVIDE:            return pimpl->m_builder.CreateSDiv(left, right, "div" );
                case NaikoOperation::MODULO:            return pimpl->m_builder.CreateSRem(left, right, "mod" );
                // Comparisons -> bool
                case NaikoOperation::GREATERTHAN:       return pimpl->m_builder.CreateICmpSGT(left, right, "gt");
                case NaikoOperation::LESSTHAN:          return pimpl->m_builder.CreateICmpSLT(left, right, "lt");
                case NaikoOperation::GREATERTHANEQUAL:  return pimpl->m_builder.CreateICmpSGE(left, right, "ge");
                case NaikoOperation::LESSTHANEQUAL:     return pimpl->m_builder.CreateICmpSLE(left, right, "le");
                case NaikoOperation::EQUALEQUAL:        return pimpl->m_builder.CreateICmpEQ(left, right, "eq");
                case NaikoOperation::NOTEQUAL:          return pimpl->m_builder.CreateICmpNE(left, right, "ne");
                default:
                AIKO_NOT_IMPLEMENTED;
            }
        }

        // FunctionNode
        if (CallExpressionNode* const call = dynamic_cast<CallExpressionNode*>(node))
        {
            llvm::Function* callee = pimpl->m_module->getFunction(call->name);
            if (callee == nullptr)
            {
                logger::Log::error("Unknown function '%s'", call->name.c_str());
                std::exit(-1);
            }
            std::vector<llvm::Value*> arguments;
            for (auto& arg : call->arguments)
            {
                arguments.push_back(emitNode(arg.get(), fnt));
            }
            return pimpl->m_builder.CreateCall(callee, arguments, "calltmp");
        }

        // UnaryOperationNode
        if (UnaryOperationNode* const un = dynamic_cast<UnaryOperationNode*>(node))
        {
            llvm::Value* unary = emitNode(un->operand.get(), fnt);
            switch (un->operation)
            {
                case NaikoOperation::SUBTRACT: return pimpl->m_builder.CreateNeg(unary);
                case NaikoOperation::ADD: return unary;
                default:
                    AIKO_NOT_IMPLEMENTED;
            }
        }

        // VariableNode
        if (VariableNode* const var = dynamic_cast<VariableNode*>(node))
        {
            auto* sym = lookupVar(var->name);
            if (llvm::AllocaInst* alloc = llvm::dyn_cast<llvm::AllocaInst>(sym->value))
            {
                return pimpl->m_builder.CreateLoad(alloc->getAllocatedType(), alloc, var->name + "_val");
            }
            if (llvm::GlobalVariable* global = llvm::dyn_cast<llvm::GlobalVariable>(sym->value))
            {
                return pimpl->m_builder.CreateLoad(global->getValueType(), global, var->name + "_val");
            }
            logger::Log::error("Unsupported variable storage type [%s]", var->name.c_str());
            std::exit(-1);
        }

        // ReturnNode
        if (ReturnNode* const ret = dynamic_cast<ReturnNode*>(node))
        {
            llvm::Value* value = emitNode(ret->expr.get(), fnt);
            return pimpl->m_builder.CreateRet(value);
        }

        AIKO_ASSERT(false, "NOT IMPLEMENTED")

    }

    llvm::Value* LlvmEmitter::getTargetPtr(ASTNode* node, llvm::Function* fnt, bool declareIfMissing)
    {
        if (VariableNode* var = dynamic_cast<VariableNode*>(node))
        {
            auto* sym = lookupVar(var->name);
            if (auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(sym->value))
            {
                return alloca;
            }
            if (auto* global = llvm::dyn_cast<llvm::GlobalVariable>(sym->value))
            {
                return global;
            }
            logger::Log::error("Unsupported variable storage type");
            std::exit(-1);
        }
        if (ArrayAccessNode* arr = dynamic_cast<ArrayAccessNode*>(node))
        {
            // get array variable name
            VariableNode* var = dynamic_cast<VariableNode*>(arr->base.get());
            if (var == nullptr)
            {
                logger::Log::error("ArrayAccessNode must have VariableNode as array"); std::exit(-1);
            }
            auto* sym = lookupVar(var->name);
            llvm::AllocaInst* alloc = llvm::dyn_cast<llvm::AllocaInst>(sym->value);
            if (alloc == nullptr)
            {
                if (declareIfMissing)
                {
                    llvm::ArrayType* arrType = llvm::ArrayType::get(pimpl->m_builder.getInt32Ty(), 100);
                    llvm::IRBuilder<> tmpBuilder(&fnt->getEntryBlock(), fnt->getEntryBlock().begin());
                    alloc = tmpBuilder.CreateAlloca(arrType, nullptr, var->name);
                    declare(var->name, alloc);
                }
                else
                {
                    logger::Log::error("Array '%s' not declared", var->name.c_str());
                    std::exit(-1);
                }
            }

            llvm::Value* index = emitNode(arr->index.get(), fnt);

            llvm::Type* allocatedType = alloc->getAllocatedType();

            if (allocatedType->isArrayTy() == true)
            {
               return pimpl->m_builder.CreateGEP(allocatedType, alloc, {pimpl->m_builder.getInt32(0), index});
            }

            if (allocatedType->isPointerTy() == true)
            {
                llvm::Value* loadPtr = pimpl->m_builder.CreateLoad(allocatedType, alloc);
                llvm::Type* elementType = pimpl->m_builder.getInt8Ty();
                return pimpl->m_builder.CreateGEP(elementType, loadPtr, index);
            }

            llvm::errs() << "Invalid array access type: ";
            allocatedType->print(llvm::errs());
            llvm::errs() << "\n";

            std::exit(-1);

        }
        AIKO_NOT_IMPLEMENTED;
    }

    llvm::Type* LlvmEmitter::toLLVMType(NaikoType type)
    {
        switch (type)
        {
            case NaikoType::INT: return pimpl->m_builder.getInt32Ty();
            case NaikoType::CHAR: return pimpl->m_builder.getInt8Ty();
            case NaikoType::STRING: return llvm::PointerType::get(pimpl->m_builder.getInt8Ty()->getContext(), 0);
            case NaikoType::VOID: return llvm::Type::getVoidTy(pimpl->m_builder.getInt8Ty()->getContext());
        default:
            logger::Log::error("Unsupported type in LLVM emitter");
            std::exit(-1);
        }
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
        pimpl->m_module->print(out, nullptr);
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

    void LlvmEmitter::emitGlobalVariable(ASTNode* node)
    {
        if (auto let = dynamic_cast<LetNode*>(node))
        {
            if (auto var = dynamic_cast<VariableNode*>(let->left.get()))
            {

                llvm::Type* ty = toLLVMType(let->right->type);

                // Create LLVM global variable
                llvm::Constant* init = nullptr;
                if (auto numNode = dynamic_cast<NumberNode*>(let->right.get()))
                {
                    init = llvm::ConstantInt::get(ty, numNode->value);
                }
                else if (auto charNode = dynamic_cast<CharNode*>(let->right.get()))
                {
                    init = llvm::ConstantInt::get(ty, charNode->value);
                }
                else
                {
                    // For now, support only char/int globals
                    logger::Log::error("Global initialization must be int or char");
                    std::exit(-1);
                }

                llvm::GlobalVariable* gv = new llvm::GlobalVariable(
                    *pimpl->m_module,
                    ty,
                    let->constant, // isConstant
                    llvm::GlobalValue::ExternalLinkage,
                    init,
                    var->name
                );

                // store in symbol table for lookup later
                declare(var->name, gv, let->constant);

            }
            else
            {
                logger::Log::error("Only simple variables can be global");
                std::exit(-1);
            }
        }
        else
        {
            logger::Log::error("Attemptting to emit global variable but is not a set command");
            std::exit(-1);
        }
    }

    void LlvmEmitter::emitFunction(FunctionNode* fnt)
    {
        // FunctionNode
        if (fnt == nullptr)
        {
            logger::Log::error("Function declaration is not a valid declaration nodel.");
            std::exit(-1);
        }

        // int function(...) for now
        std::vector<llvm::Type*> paramTypes;
        for (auto& param : fnt->parameters)
        {
            paramTypes.push_back(toLLVMType(param.type));
        }
        llvm::Type* returnType = toLLVMType(fnt->returnType);
        llvm::FunctionType* fntType = llvm::FunctionType::get( returnType, paramTypes, false);

        llvm::Function* function = llvm::Function::Create(fntType, llvm::Function::ExternalLinkage, fnt->name, pimpl->m_module.get());

        // name parameters
        unsigned int index = 0;
        for (auto& arg : function->args())
        {
            arg.setName(fnt->parameters[index++].name);
        }

        // entry block
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(pimpl->m_context, "entry", function);
        pimpl->m_builder.SetInsertPoint(entry);

        enterScope();

        // allocate parameters
        for (auto& arg : function->args())
        {
            llvm::IRBuilder<> tmpBuilder(&function->getEntryBlock(), function->getEntryBlock().begin());
            llvm::AllocaInst* alloc = tmpBuilder.CreateAlloca(arg.getType(), nullptr, arg.getName());
            pimpl->m_builder.CreateStore(&arg, alloc);
            declare(arg.getName().str(), alloc);
        }

        // emit body
        for (auto& stmt : fnt->body)
        {
            emitNode(stmt.get(), function);
        }

        // default return if none emitted
        if (pimpl->m_builder.GetInsertBlock()->getTerminator() == nullptr)
        {
            if (function->getReturnType()->isVoidTy() == true)
            {
                pimpl->m_builder.CreateRetVoid();
            }
            else
            {
                pimpl->m_builder.CreateRet(llvm::ConstantInt::get(function->getReturnType(), 0));
            }
        }

        exitScope();

        if (llvm::verifyFunction(*function, &llvm::errs()))
        {
            logger::Log::error("Function verification failed: %s", fnt->name.c_str());
            std::exit(-1);
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

    void LlvmEmitter::declare(const string name, llvm::Value* fnt, bool isConstant)
    {
        m_scopeStack.back().insert({ name, { fnt, isConstant } });
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

    LlvmEmitter::LlvmSymbol* LlvmEmitter::lookupVar(const string name)
    {
        for (auto it = m_scopeStack.rbegin(); it != m_scopeStack.rend(); ++it)
        {
            if (it->contains(name) == true)
            {
                return &it->at(name);
            }
        }
        logger::Log::error("Variable '%s' not declared", name.c_str());
        std::exit(-1);
        return nullptr;
    }
}
