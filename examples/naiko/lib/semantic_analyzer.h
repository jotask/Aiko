#pragma once

#include "parser_nodes.h"

namespace aiko::naiko
{

    class SemanticAnalyzer
    {
    public:
        void analyze(ProgramNode* node);

    private:

        struct Symbol
        {
            NaikoType type = NaikoType::UNKNOWN;
            bool isArray = false;
            bool isFunction = false;
            std::vector<NaikoType> params;
        };

        using Scope = std::unordered_map<string, Symbol>;
        std::vector<Scope> m_scopes;

        // Scope helpers
        void enterScope();
        void exitScope();
        void declare(const string& name, Symbol sm);
        Symbol* lookUp(const string& name);

        // helper
        void unify(NaikoType& a, NaikoType& b);
        NaikoType m_currentFunctionReturn = NaikoType::INVALID;

        // Visitors
        void analyzeNode(ASTNode* node);
        NaikoType analyzeExpr(ASTNode* node);

    };

}
