#pragma once

#include <aiko.h>

namespace aiko::naiko
{

    #define NEW_LINE '\n'

    enum class Keywords
    {
        LET,
        RETURN
    };

    enum class TokenKind
    {
        END,
        INVALID,
        PREPROCESSOR,
        SYMBOL,
        COMMENT,
        KEYWORD,
        SEMICOLON,
        OPEN_PARENT,
        CLOSE_PARENT,
        OPEN_CURLY,
        CLOSE_CURLY,
        STRING,
        OPERATOR,
        DIGIT,
    };

    struct Token
    {
        TokenKind kind;
        string text;
        size_t position;
        size_t line;
    };

}