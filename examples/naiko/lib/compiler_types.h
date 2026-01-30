#pragma once

#include <aiko.h>
#include <variant>

#include <vector>

#include <magic_enum/magic_enum_all.hpp>

namespace aiko::naiko
{

    #define NEW_LINE '\n'

    enum class TokenKind
    {
        INVALID,
        END,
        PREPROCESSOR,
        IDENTIFIER,
        SYMBOL,
        COMMENT,
        KEYWORD,
        OPERATOR,
        VALUE,
    };

    enum class NaikoKeyword : uint8_t
    {
        INVALID,
        WHILE,
        ENDWHILE,
        INPUT,
        REPEAT,
        RETURN,
        LET,
        SET,
        IF,
        THEN,
        ENDIF,
        PRINT,
    };

    enum class NaikoSymbol : uint8_t
    {
        INVALID,
        OPEN_PARENT,
        CLOSE_PARENT,
        OPEN_CURLY,
        CLOSE_CURLY,
        OPEN_SQUARE,
        CLOSE_SQUARE,
    };


    enum class NaikoOperation : uint8_t
    {
        INVALID,
        LESSTHAN,
        GREATERTHAN,
        EQUAL,
        LESSTHANEQUAL,
        GREATERTHANEQUAL,
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        MODULO
    };

    enum class NaikoType : uint8_t
    {
        INVALID,
        STRING,
        DIGIT
    };

    using Naiko = std::variant<std::monostate, NaikoKeyword, NaikoSymbol, NaikoOperation, NaikoType>;

    struct Token
    {
        TokenKind kind;
        Naiko  naiko;
        string text;
        size_t position;
        size_t line;
    };

    using Tokenization = std::vector<Token>;

    static NaikoKeyword getKeywordKind(string str)
    {
        auto value = magic_enum::enum_cast<NaikoKeyword>(str);
        AIKO_ASSERT(value.has_value(), "Unknown keyword");
        return value.value_or(NaikoKeyword::INVALID);
    }

    static NaikoOperation getOperationKind(string str)
    {

        static const std::unordered_map<std::string, NaikoOperation> table = {
            {"<",  NaikoOperation::LESSTHAN},
            {">",  NaikoOperation::GREATERTHAN},
            {"=",  NaikoOperation::EQUAL},
            {"<=", NaikoOperation::LESSTHANEQUAL},
            {">=", NaikoOperation::GREATERTHANEQUAL},
            {"+",  NaikoOperation::ADD},
            {"-",  NaikoOperation::SUBTRACT},
            {"*",  NaikoOperation::MULTIPLY},
            {"/",  NaikoOperation::DIVIDE},
            {"%",  NaikoOperation::MODULO}
        };

        const auto it = table.find(str);
        if (it != table.end())
        {
            return it->second;
        }

        AIKO_ASSERT(false, "Unknown operation");
        return NaikoOperation::INVALID;
    }

    static NaikoType getTypeKind(string str)
    {
        if(static_cast<bool>(std::isdigit(str.front())))
        {
            return NaikoType::DIGIT;
        }
        if (str.front() == '\"')
        {
            return NaikoType::STRING;
        }
        return NaikoType::INVALID;
    }

}