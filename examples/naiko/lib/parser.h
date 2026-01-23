#pragma once

#include <aiko.h>

#include "compiler_types.h"

namespace aiko::naiko
{

    class Parser
    {

    public:

        Parser(const Tokenization);
        ~Parser() = default;

        void program();

    private:

        void setup();

        void next();
        void match(TokenKind);
        void match(TokenKind, Naiko naiko);
        bool checkCurrent(TokenKind);
        bool checkNext(TokenKind);

        Token getCurrentToken() const;
        Token getNextToken() const;

        void statement();

        void processKeyword();
        void processComparison();

        Tokenization m_tokens;
        size_t m_current;

    };

}