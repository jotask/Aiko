#pragma once

#include <functional>

#include <aiko.h>
#include "compiler_types.h"
#include "naiko_compiler_options.h"

namespace aiko::naiko
{

    class Lexer
    {
    public:
        Lexer(string code);
        Lexer(const CompilerOptions opts);
        ~Lexer() = default;

        Token next();

    private:

        using FntUntilCompare = std::function<bool()>;

        void readInputFiles(std::vector<string>);

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
