#pragma once

#include "compiler_types.h"
#include "parser_nodes.h"

#include <logger/logger.h>
#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    static string escapeChar(const char c)
    {
        switch (c)
        {
        case '\0': return "\\0";
        case '\n': return "\\n";
        case '\t': return "\\t";
        case '\r': return "\\r";
        case '\\': return "\\\\";
        case '\'': return "\\'";
        default:
            if (std::isprint(static_cast<unsigned char>(c)))
            {
                return std::string(1, c);
            }
            return "\\x" + std::to_string(static_cast<int>(c));
        }
    }

    inline void printToken(const Token token)
    {
        if (std::holds_alternative<std::monostate>(token.naiko) == false)
        {

            if (std::holds_alternative<NaikoKeyword>(token.naiko))
            {
                auto na = std::get<NaikoKeyword>(token.naiko);
                logger::Log::info("Token: %s -> %s : %s", magic_enum::enum_name(token.kind).data(), magic_enum::enum_name(na).data(), token.text.c_str());
            }

            if (std::holds_alternative<NaikoOperation>(token.naiko))
            {
                auto na = std::get<NaikoOperation>(token.naiko);
                logger::Log::info("Token: %s -> %s : %s", magic_enum::enum_name(token.kind).data(), magic_enum::enum_name(na).data(), token.text.c_str());
            }

            if (std::holds_alternative<NaikoType>(token.naiko))
            {
                auto na = std::get<NaikoType>(token.naiko);
                logger::Log::info("Token: %s -> %s : %s", magic_enum::enum_name(token.kind).data(), magic_enum::enum_name(na).data(), token.text.c_str());
            }

            if (std::holds_alternative<NaikoSymbol>(token.naiko))
            {
                auto na = std::get<NaikoSymbol>(token.naiko);
                logger::Log::info("Token: %s -> %s : %s", magic_enum::enum_name(token.kind).data(), magic_enum::enum_name(na).data(), token.text.c_str());
            }

        }
        else
        {
            logger::Log::info("Token: %s -> %s", magic_enum::enum_name(token.kind).data(), token.text.c_str());
        }
    }

    inline string makeIndent(const size_t indent)
    {
        constexpr size_t spacesPerLevel = 4;
        return string(indent * spacesPerLevel, ' ');
    }

}