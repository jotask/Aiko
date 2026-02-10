#include "semantic_analyzer.h"

#include <logger/logger.h>

#include "parser_nodes.h"

namespace aiko::naiko
{
    void SemanticAnalyzer::analyze(ProgramNode* node)
    {
        enterScope();
        for (auto& stmt : node->statements)
        {
            analyzeNode(stmt.get());
        }
        exitScope();
    }

    void SemanticAnalyzer::enterScope()
    {
        m_scopes.emplace_back();
    }

    void SemanticAnalyzer::exitScope()
    {
        m_scopes.pop_back();
    }

    void SemanticAnalyzer::declare(const string& name, Symbol sym)
    {
        if (m_scopes.empty())
        {
            m_scopes.push_back({});
        }
        auto& scope = m_scopes.back();
        if (scope.contains(name) == true)
        {
            logger::Log::error("Variable already declared in this scope: %s", name.c_str());
            std::exit(-1);
        }
        scope[name] = sym;
    }


    SemanticAnalyzer::Symbol* SemanticAnalyzer::lookUp(const string& name)
    {
        for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it )
        {
            if (it->contains(name) == true)
            {
                return &it->at(name);
            }
        }
        return nullptr;
    }

    void SemanticAnalyzer::analyzeNode(ASTNode* node)
    {
        if (node == nullptr)
        {
            return;
        }

        // LET
        if (auto let = dynamic_cast<LetNode*>(node))
        {
            auto expr = analyzeExpr(let->right.get());
            if (auto arr = dynamic_cast<ArrayAccessNode*>(let->left.get()))
            {
                declare(arr->name, {NaikoType::ARRAY, true});
                node->type = NaikoType::VOID;
                return;
            }
            if (auto var = dynamic_cast<VariableNode*>(let->left.get()))
            {
                declare(var->name, {expr, false});
                node->type = NaikoType::VOID;
                return;
            }
            logger::Log::error("Unknows let expression on let node");
            std::exit(-1);
        }

        // SET
        if (auto set = dynamic_cast<SetNode*>(node))
        {
            auto variable = dynamic_cast<VariableNode*>(set->left.get());
            if (variable == nullptr)
            {
                logger::Log::error("Expected variable node on set, but found other thing.");
                std::exit(-1);
            }
            auto sym = lookUp(variable->name);
            if (sym == nullptr)
            {
                logger::Log::error("Assignment to undeclared variable.");
                std::exit(-1);
            }

            auto rhsType = analyzeExpr(set->right.get());
            if (sym->type != rhsType)
            {
                logger::Log::error("Type mismatch in assignment.");
                std::exit(-1);
            }

            node->type = NaikoType::VOID;
            return;
        }

        // PRINT
        if (auto print = dynamic_cast<PrintNode*>(node))
        {
            analyzeExpr(print->expr.get());
            node->type = NaikoType::VOID;
            return;
        }

        // IF
        if (auto ifNode = dynamic_cast<IfNode*>(node))
        {
            if (analyzeExpr(ifNode->condition.get()) != NaikoType::BOOL)
            {
                logger::Log::error("If condition does not evaluate to bool");
                std::exit(-1);
            }
            enterScope();
            for (auto& stmt : ifNode->body)
            {
                analyzeNode(stmt.get());
            }
            exitScope();
            node->type = NaikoType::VOID;
            return;
        }

        // WHILE
        if (auto whileNode = dynamic_cast<WhileNode*>(node))
        {
            if (analyzeExpr(whileNode->condition.get()) != NaikoType::BOOL)
            {
                logger::Log::error("If condition does not evaluate to bool");
                std::exit(-1);
            }
            enterScope();
            for (auto& stmt : whileNode->body)
            {
                analyzeNode(stmt.get());
            }
            exitScope();
            node->type = NaikoType::VOID;
            return;
        }

        // FUNCTION
        if (FunctionNode* fnt = dynamic_cast<FunctionNode*>(node))
        {

            Symbol fnSym = {
                .type = NaikoType::INT,
                .isFunction = true,
            };
            fnSym.type = NaikoType::INT; // or VOID / UNKNOWN for now
            fnSym.isFunction = true;

            for (size_t i = 0; i < fnt->parameters.size(); ++i)
            {
                fnSym.params.push_back(NaikoType::INT); // assume int params for now
            }

            declare(fnt->name, fnSym);

            enterScope();
            for (auto& param : fnt->parameters)
            {
                declare(param.name, {param.type, false}); // or UNKNOWN
            }
            for (auto& stmt : fnt->body)
            {
                analyzeNode(stmt.get());
            }
            exitScope();
            node->type = NaikoType::VOID;
            return;
        }

        // fallback = expression
        analyzeExpr(node);

    }

    NaikoType SemanticAnalyzer::analyzeExpr(ASTNode* node)
    {
        if (node == nullptr)
        {
            return NaikoType::INVALID;
        }

        if (NumberNode* n = dynamic_cast<NumberNode*>(node))
        {
            node->type = NaikoType::INT;
            return node->type;
        }

        if (CharNode* n = dynamic_cast<CharNode*>(node))
        {
            node->type = NaikoType::CHAR;
            return node->type;
        }

        if (StringNode* n = dynamic_cast<StringNode*>(node))
        {
            node->type = NaikoType::STRING;
            return node->type;
        }

        if (VariableNode* n = dynamic_cast<VariableNode*>(node))
        {
            auto sym = lookUp(n->name);
            if (sym == nullptr)
            {
                logger::Log::error("Used of undeclared variable: %s", n->name.c_str());
                std::exit(-1);
            }
            node->type = sym->type;
            return node->type;
        }

        if (ArrayAccessNode* n = dynamic_cast<ArrayAccessNode*>(node))
        {
            auto sym = lookUp(n->name);
            if (sym == nullptr)
            {
                logger::Log::error("Used of undeclared variable: %s", n->name.c_str());
                std::exit(-1);
            }
            auto expr = analyzeExpr(n->index.get());
            if (expr != NaikoType::INT)
            {
                logger::Log::error("Array index must be INT");
                std::exit(-1);
            }

            // array access
            if (sym->isArray == true)
            {
                node->type = NaikoType::INT; // or elementType later
                return node->type;
            }

            if (sym->type == NaikoType::STRING)
            {
                node->type = NaikoType::CHAR; // CHAR if you add it later
                return node->type;
            }

            logger::Log::error("Type '%s' is not indexable", n->name.c_str());
            std::exit(-1);

        }

        if (BinaryOperationNode* n = dynamic_cast<BinaryOperationNode*>(node))
        {
            auto left = analyzeExpr(n->left.get());
            auto right = analyzeExpr(n->right.get());

            switch (n->operation)
            {
            case NaikoOperation::ADD:
            case NaikoOperation::SUBTRACT:
                if (left == NaikoType::STRING && right == NaikoType::STRING)
                {
                    node->type = NaikoType::STRING;
                    return node->type;
                }
                if (left == NaikoType::INT && right == NaikoType::INT)
                {
                    node->type = NaikoType::INT;
                    return node->type;
                }
                break;
            case NaikoOperation::EQUAL:
            case NaikoOperation::GREATERTHAN:
            case NaikoOperation::GREATERTHANEQUAL:
            case NaikoOperation::LESSTHAN:
            case NaikoOperation::LESSTHANEQUAL:
                {
                    if (left != right)
                    {
                        logger::Log::error("Type mismatch in comparison");
                        std::exit(-1);
                    }
                    node->type = NaikoType::BOOL;
                    return node->type;
                }
                break;
            }
            auto op = magic_enum::enum_name(n->operation);
            logger::Log::error("Invalid binary operation: [%s]", op.data());
            std::exit(-1);
        }

        if (UnaryOperationNode* n = dynamic_cast<UnaryOperationNode*>(node))
        {
            auto expr = analyzeExpr(n->operand.get());
            if (expr != NaikoType::INT)
            {
                logger::Log::error("Unary op expects int");
                std::exit(-1);
            }
            node->type = NaikoType::INT;
            return node->type;
        }

        if (CallExpressionNode* expr = dynamic_cast<CallExpressionNode*>(node))
        {
            auto sym = lookUp(expr->name);
            if (sym == nullptr || sym->isFunction == false)
            {
                logger::Log::error("Call to undefined function: %s", expr->name.c_str());
                std::exit(-1);
            }

            if (sym->params.size() != expr->arguments.size())
            {
                logger::Log::error("Function '%s' expects %zu args, got %zu",
                    expr->name.c_str(),
                    sym->params.size(),
                    expr->arguments.size());
                std::exit(-1);
            }

            for (size_t i = 0; i < expr->arguments.size(); ++i)
            {
                auto argType = analyzeExpr(expr->arguments[i].get());
                if (argType != sym->params[i])
                {
                    logger::Log::error("Argument %zu type mismatch in call to '%s'",
                        i, expr->name.c_str());
                    std::exit(-1);
                }
            }

            node->type  = sym->type;
            return node->type;

        }

        if (ReturnNode* ret = dynamic_cast<ReturnNode*>(node))
        {
            auto expr = analyzeExpr(ret->expr.get());
            node->type = expr;
            return node->type;
        }

        logger::Log::error("Unknown expression");
        return NaikoType::INVALID;

    }

}
