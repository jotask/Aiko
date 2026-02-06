#pragma once

#include "parser_nodes.h"

namespace aiko::naiko
{

    class SemanticAnalyzer
    {
    public:
        void analyze(ProgramNode* node);

    private:

        struct Symbol {
            NaikoType type;
            bool isArray = false;
        };

        using Scope = std::unordered_map<string, Symbol>;
        std::vector<Scope> m_scopes;

        // Scope helpers
        void enterScope();
        void exitScope();
        void declare(const string& name, Symbol sm);
        Symbol* lookUp(const string& name);

        // Visitors
        void analyzeNode(ASTNode* node);
        NaikoType analyzeExpr(ASTNode* node);

    };

}
