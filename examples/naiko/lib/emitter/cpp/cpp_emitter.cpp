#include "cpp_emitter.h"

#include "compiler_helper.h"

namespace aiko::naiko
{
    CppEmitter::CppEmitter(string file)
        : Emitter(file)
    {

    }

    void CppEmitter::emit(ProgramNode* node)
    {

        clear();

        newLine("#include <iostream>");
        newLine("#include <cstdlib>");

        newLine();
        newLine("int main()");
        newLine("{");
        for (NodePtr& stmts : node->statements)
        {
            emitNode(stmts.get(), 1);
        }
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
        if (auto end = dynamic_cast<EndNode*>(node))
        {
            return;
        }

        // PRINT
        if (auto print = dynamic_cast<PrintNode*>(node))
        {
            append(makeIndent(indent));
            append("std::cout <<");
            emitNode(print->expr.get(), 0);
            append("<< std::endl");
            append(";");
            newLine();
            return;
        }

        // STRING
        if (auto str = dynamic_cast<StringNode*>(node))
        {
            append(makeIndent(indent));
            append(" \"");
            append(str->value);
            append("\" ");
            return;
        }

        // NUMBER
        if (auto num = dynamic_cast<NumberNode*>(node))
        {
            append(makeIndent(indent));
            append(std::to_string(num->value));
            return;
        }

        // LET
        if (auto let = dynamic_cast<LetNode*>(node))
        {
            append(makeIndent(indent));
            append("auto ");
            append(let->symbol.c_str());
            append(" = ");
            emitNode(let->expr.get(), 0);
            append(";");
            newLine();
            return;
        }

        // IF
        if (auto ifN = dynamic_cast<IfNode*>(node))
        {
            append(makeIndent(indent));
            append("if ( ");
            emitNode(ifN->condition.get(), 0);
            append(" )");
            newLine();
            append(makeIndent(indent));
            append("{");
            newLine();
            for (auto& b : ifN->body)
            {
                emitNode(b.get(), indent + 1);
            }
            newLine();
            append(makeIndent(indent));
            append("}");
            newLine();
            return;
        }

        // WHILE
        if (auto whileN = dynamic_cast<WhileNode*>(node))
        {
            append(makeIndent(indent));
            append("while ( ");
            emitNode(whileN->condition.get(), 0);
            append(" )");
            newLine();
            append(makeIndent(indent));
            append("{");
            newLine();
            for (auto& b : whileN->body)
            {
                emitNode(b.get(), indent + 1);
            }
            newLine();
            append(makeIndent(indent));
            append("}");
            newLine();
            return;
        }

        // BinaryOperationNode
        if (auto bin = dynamic_cast<BinaryOperationNode*>(node))
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
        if (auto un = dynamic_cast<UnaryOperationNode*>(node))
        {
            return;
        }

        // VariableNode
        if (auto var = dynamic_cast<VariableNode*>(node))
        {
            append(var->name);
            return;
        }

        AIKO_ASSERT(false, "NOT IMPLEMENTED")

    }
}
