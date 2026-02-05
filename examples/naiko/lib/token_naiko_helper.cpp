#include "token_naiko_helper.h"

#include <magic_enum/magic_enum.hpp>

#include <logger/logger.h>

namespace aiko::naiko::helper
{

    void resolveNaiko(Token& token)
    {
        switch (token.kind)
        {
        case TokenKind::KEYWORD:    token.naiko = resolveKeyword(token);    break;
        case TokenKind::SYMBOL:     token.naiko = resolveSymbol(token);     break;
        case TokenKind::OPERATOR:   token.naiko = resolveOperation(token);  break;
        case TokenKind::VALUE:      token.naiko = resolveType(token);       break;
        default:                    break;
        }
    }

    NaikoKeyword resolveKeyword(Token& token)
    {
        AIKO_ASSERT(token.kind == TokenKind::KEYWORD, "Unsupported keyword");
        auto result = magic_enum::enum_cast<NaikoKeyword>(token.text);
        if (result.has_value() == false)
        {
            logger::Log::error("Unknow keyword type");
            std::exit(-1);
        }
        return result.value_or(NaikoKeyword::INVALID);
    }

    NaikoSymbol resolveSymbol(Token& token)
    {
        const auto str = token.text;
        if (str == "[")     return NaikoSymbol::OPEN_SQUARE;
        if (str == "]")     return NaikoSymbol::CLOSE_SQUARE;
        if (str == "{")     return NaikoSymbol::OPEN_CURLY;
        if (str == "}")     return NaikoSymbol::CLOSE_CURLY;
        if (str == "(")     return NaikoSymbol::OPEN_PARENT;
        if (str == ")")     return NaikoSymbol::CLOSE_PARENT;
        AIKO_ASSERT(false, "Not supported");
        return NaikoSymbol::INVALID;
    }

    NaikoOperation resolveOperation(Token& token)
    {
        const auto str = token.text;
        if (str == "<")     return NaikoOperation::LESSTHAN;
        if (str == ">")     return NaikoOperation::GREATERTHAN;
        if (str == "=")     return NaikoOperation::EQUAL;
        if (str == "<=")    return NaikoOperation::LESSTHANEQUAL;
        if (str == ">=")    return NaikoOperation::GREATERTHANEQUAL;
        if (str == "+")     return NaikoOperation::ADD;
        if (str == "-")     return NaikoOperation::SUBTRACT;
        if (str == "*")     return NaikoOperation::MULTIPLY;
        if (str == "/")     return NaikoOperation::DIVIDE;
        if (str == "%")     return NaikoOperation::MODULO;
        AIKO_ASSERT(false, "Not supported");
        return NaikoOperation::INVALID;
    }

    NaikoType resolveType(Token& token)
    {
        if (std::isdigit(token.text.front()))       return NaikoType::DIGIT;
        return NaikoType::STRING;
    }

}
