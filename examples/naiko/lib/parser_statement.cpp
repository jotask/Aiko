#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    template<class T>
    static void printStatement(NaikoKeyword key, T var, string text)
    {
        const auto name = magic_enum::enum_name(key);
        logger::Log::warning("STATEMENT-%s-STRING: [%s]", name.data(), text.c_str());
    }

    void Parser::processKeyword()
    {
        const auto current = getCurrentToken();
        if (const NaikoKeyword* keyword = std::get_if<NaikoKeyword>(&current.naiko))
        {
            switch (*keyword)
            {
            case NaikoKeyword::PRINT:
                {
                    next();
                    const auto value = getCurrentToken();
                    if (value.kind == TokenKind::VALUE)
                    {
                        if (const NaikoType* type = std::get_if<NaikoType>(&value.naiko))
                        {
                            switch (*type)
                            {
                                case NaikoType::STRING:
                                    {
                                        printStatement<NaikoType>(*keyword, *type, value.text);
                                        next();
                                        return;
                                    }
                                break;
                                case NaikoType::DIGIT:
                                {
                                    printStatement<NaikoType>(*keyword, *type, value.text);
                                    next();
                                    return;
                                }
                                break;
                                default:
                                    AIKO_ASSERT(false, "Not implemented print for this type");
                            }
                        }
                        else
                        {
                            AIKO_ASSERT(false, "Unknow type for print keyword")
                        }
                    }
                    else
                    {
                        statement();
                    }
                }
                break;
            case NaikoKeyword::IF:
                {
                    processComparison();
                    match(TokenKind::KEYWORD, NaikoKeyword::THEN);
                    return;
                }
                break;
            default:
                AIKO_ASSERT(false, "Not Implemented KEYWORD");
            }
        }
        else
        {
            AIKO_ASSERT(false, "Found an unknow keyword?");
        }

    }

}