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

    static char unescapeChar(string str)
    {
        if (str.empty())
        {
            logger::Log::error("Empty string cannot be unescaped");
            std::exit(-1);
        }

        if (str[0] != '\\')
        {
            if (str.size() == 1)
            {
                return str[0];
            }
            logger::Log::error("Invalid escape sequence: %s", str);
            std::exit(-1);
        }

        if (str.size() == 2)
        {
            switch (str[1]) {
            case '0': return '\0';
            case 'n': return '\n';
            case 't': return '\t';
            case 'r': return '\r';
            case '\\': return '\\';
            case '\'': return '\'';
            default:
                logger::Log::error("Unknown escape sequence: %s", str);
                std::exit(-1);
            }
        }

        // Hex escape sequence: \xNN
        if (str.size() >= 4 && str[1] == 'x')
        {
            int value = 0;
            std::istringstream iss(str.substr(2));
            iss >> std::hex >> value;
            if (iss.fail() || value < 0 || value > 255)
            {
                logger::Log::error("Invalid hex escape: %s", str);
                std::exit(-1);
            }
            return static_cast<char>(value);
        }
        logger::Log::error("Unsupported escape sequence: %s", str);
        std::exit(-1);
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