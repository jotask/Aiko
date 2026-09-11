#pragma once

#include <functional>

#include "compiler_types.h"

namespace aiko::naiko
{

    class Lexer
    {
    public:
        Lexer(string code);
        ~Lexer() = default;

        Token next();

    private:

        using FntUntilCompare = std::function<bool()>;

        void chopChar();

        void skipUntil(FntUntilCompare);
        void skipWhitespace();

        bool isKeyword(string str) const;
        bool isSymbol(const char) const;
        bool isOperator(const char) const;

        using IsFnt = int(*)(int);
        bool isFunction(IsFnt, char);

        string getStr(size_t start, size_t end) const;

        string m_code;
        size_t m_cursor;
        size_t m_line;

    };

}
