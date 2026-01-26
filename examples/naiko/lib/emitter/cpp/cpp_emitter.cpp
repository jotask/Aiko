#include "cpp_emitter.h"

#include "compiler_helper.h"

#include <fstream>

namespace aiko::naiko
{
    CppEmitter::CppEmitter(string file)
        : Emitter(file + ".cpp")
    {

    }

    void CppEmitter::append(string text)
    {
        m_code.append(text);
    }

    void CppEmitter::newLine(string line)
    {
        append(line);
        append("\n");
    }

    void CppEmitter::emit(ProgramNode* node)
    {

        m_code.clear();

        newLine("#include <iostream>");
        newLine("#include <cstdlib>");

        newLine();
        newLine("int main()");
        newLine("{");

        enterScope(); // global scope
        for (NodePtr& stmts : node->statements)
        {
            emitNode(stmts.get(), 1);
        }
        exitScope();
        newLine("    return EXIT_SUCCESS;");
        newLine("}");
        newLine();

        save();
    }

    void CppEmitter::emitNode(ASTNode* node, size_t indent)
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
            append(makeIndent(indent));
            append("std::cout << ");
            emitNode(print->expr.get(), 0);
            append(" << std::endl");
            append(";");
            newLine();
            return;
        }

        // STRING
        if (StringNode* const str = dynamic_cast<StringNode*>(node))
        {
            append(makeIndent(indent));
            append("\"");
            append(str->value);
            append("\"");
            return;
        }

        // NUMBER
        if (NumberNode* const num = dynamic_cast<NumberNode*>(node))
        {
            append(makeIndent(indent));
            append(std::to_string(num->value));
            return;
        }

        // LET
        if (LetNode* const let = dynamic_cast<LetNode*>(node))
        {
            append(makeIndent(indent));
            if (isDeclared(let->symbol) == false)
            {
                append("auto ");
                declare(let->symbol);
            }
            append(let->symbol.c_str());
            append(" = ");
            emitNode(let->expr.get(), 0);
            append(";");
            newLine();
            return;
        }

        // IF
        if (IfNode* const ifN = dynamic_cast<IfNode*>(node))
        {
            append(makeIndent(indent));
            append("if ( ");
            emitNode(ifN->condition.get(), 0);
            append(" )");
            newLine();
            append(makeIndent(indent));
            append("{");
            newLine();
            enterScope();
            for (auto& b : ifN->body)
            {
                emitNode(b.get(), indent + 1);
            }
            exitScope();
            newLine();
            append(makeIndent(indent));
            append("}");
            newLine();
            return;
        }

        // WHILE
        if (WhileNode* const whileN = dynamic_cast<WhileNode*>(node))
        {
            append(makeIndent(indent));
            append("while ( ");
            emitNode(whileN->condition.get(), 0);
            append(" )");
            newLine();
            append(makeIndent(indent));
            append("{");
            newLine();
            enterScope();
            for (auto& b : whileN->body)
            {
                emitNode(b.get(), indent + 1);
            }
            newLine();
            append(makeIndent(indent));
            append("}");
            newLine();
            exitScope();
            return;
        }

        // BinaryOperationNode
        if (BinaryOperationNode* const bin = dynamic_cast<BinaryOperationNode*>(node))
        {
            append(makeIndent(0));
            if (bin->left != nullptr)
            {
                emitNode(bin->left.get(), 0);
            }
            append(" ");
            switch (bin->operation)
            {
                case NaikoOperation::EQUAL : append("="); break;
                case NaikoOperation::GREATERTHAN : append(">"); break;
                case NaikoOperation::MULTIPLY : append("*"); break;
                case NaikoOperation::DIVIDE : append("/"); break;
                case NaikoOperation::SUBTRACT : append("-"); break;
                case NaikoOperation::ADD : append("+"); break;
                default:
                AIKO_ASSERT(false, "NOT IMPLEMENTED");
            }
            append(" ");
            if (bin->right != nullptr)
            {
                emitNode(bin->right.get(), 0);
            }
            return;
        }

        // UnaryOperationNode
        if (UnaryOperationNode* const un = dynamic_cast<UnaryOperationNode*>(node))
        {
            return;
        }

        // VariableNode
        if (VariableNode* const var = dynamic_cast<VariableNode*>(node))
        {
            append(var->name);
            return;
        }

        AIKO_ASSERT(false, "NOT IMPLEMENTED")

    }

    void CppEmitter::save()
    {
        std::ofstream file(m_file);
        file << m_code;
    }

    void CppEmitter::enterScope()
    {
        m_scopeStack.emplace_back();
    }

    void CppEmitter::exitScope()
    {
        m_scopeStack.pop_back();
    }

    void CppEmitter::declare(const string name)
    {
        m_scopeStack.back().insert(name);
    }

    bool CppEmitter::isDeclared(const string name)
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

}
