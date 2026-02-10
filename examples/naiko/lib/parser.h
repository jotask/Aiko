#pragma once

#include "compiler_types.h"
#include "parser_nodes.h"

namespace aiko::naiko
{

    class Parser
    {

    public:

        Parser(const Tokenization);
        ~Parser() = default;

        NodeUPtr<ProgramNode> program();

    private:

        void setup();

        void next();
        void match(TokenKind);
        void match(TokenKind, Naiko naiko);
        bool checkCurrent(TokenKind);

        const Token& getCurrentToken() const;
        Token& getCurrentToken();

        template<class T>
        bool checkCurrent(TokenKind, T) const;

        template<class T>
        bool isTokenMatch(Token token, TokenKind kind, T naiko) const;

        NodePtr processKeyword();
        NodePtr processStatement();

        NodePtr processExpression();        // comparison
        NodePtr processComparison();        // term ( ( < | > | == ) term )*
        NodePtr processTerm();              // unary ( ( + | - ) unary )*
        NodePtr processUnary();             // ( - | ! ) unary | primary
        NodePtr processPostfix();
        NodePtr processPrimary();           // NUMBER | STRING | IDENTIFIER | '(' expression ')'

        bool isComparisonOperator() const;

        Tokenization m_tokens;
        size_t m_current;

    };

    template <class T>
    bool Parser::checkCurrent(TokenKind kind, T naiko) const
    {
        const auto current = getCurrentToken();
        if (current.kind != kind)
        {
            return false;
        }
        if (const T* item = std::get_if<T>(&current.naiko))
        {
            return *item == naiko;
        }
        return false;
    }

    template<class T>
    bool Parser::isTokenMatch(Token token, TokenKind kind, T naiko) const
    {
        if (token.kind != kind)
        {
            return false;
        }

        if constexpr (std::is_same_v<T, NaikoKeyword>)
        {
            if (std::holds_alternative<NaikoKeyword>(token.naiko))
            {

                return std::get<NaikoKeyword>(token.naiko) == naiko;
            }
        }
        else if constexpr (std::is_same_v<T, NaikoOperation>)
        {
            if (std::holds_alternative<NaikoOperation>(token.naiko))
            {
                return std::get<NaikoOperation>(token.naiko) == naiko;
            }
        }
        else if constexpr (std::is_same_v<T, NaikoType>)
        {
            if (std::holds_alternative<NaikoType>(token.naiko))
            {
                return std::get<NaikoType>(token.naiko) == naiko;
            }
        }
        else if constexpr (std::is_same_v<T, NaikoSymbol>)
        {
            if (std::holds_alternative<NaikoSymbol>(token.naiko))
            {
                return std::get<NaikoSymbol>(token.naiko) == naiko;
            }
        }
        AIKO_ASSERT(false, "Coulnd't parse naiko");
        return false;
    }

}
