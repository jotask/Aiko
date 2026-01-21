#pragma once

#include <functional>

#include <aiko.h>
#include "compiler_types.h"

namespace aiko::naiko
{

    class Lexer
    {
    public:
        Lexer(const string code);
        ~Lexer() = default;

        Token next();

    private:

        using FntUntilCompare = std::function<bool()>;

        void chopChar(size_t many = 1);

        void skipUntil(FntUntilCompare);
        void skipUntilWhitespace();
        void skipWhitespace();

        bool isFirstCharacter(string, unsigned char) const;

        bool isPreprocessor(string str) const;
        bool isKeyword(string str) const;
        bool isSymbol(string str) const;
        bool isOperator(string str) const;
        bool isDigit(string str) const;

        string getStr(size_t start, size_t end) const;

        const string m_code;

        size_t m_cursor;
        size_t m_line;

    };

}
