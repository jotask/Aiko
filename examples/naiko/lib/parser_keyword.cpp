#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    NodePtr Parser::processKeyword()
    {
        const auto current = getCurrentToken();
        if (const NaikoKeyword* keyword = std::get_if<NaikoKeyword>(&current.naiko))
        {
            switch (*keyword)
            {
            case NaikoKeyword::PRINT:
                {
                    logger::Log::warning("%s", "STATEMENT-PRINT");
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

            case NaikoKeyword::LET:
                {
                    logger::Log::warning("%s", "STATEMENT-LET");
                    next();
                    match(TokenKind::SYMBOL);
                    match(TokenKind::OPERATOR, NaikoOperation::EQUAL);
                    processExpression();
                }
                break;
            case NaikoKeyword::IF:
                {
                    logger::Log::warning("%s", "STATEMENT-IF");
                    next();
                    processComparison();
                    match(TokenKind::KEYWORD, NaikoKeyword::THEN);
                    while (isTokenMatch(getCurrentToken(), TokenKind::KEYWORD, NaikoKeyword::ENDIF) == false)
                    {
                        statement();
                    }
                    match(TokenKind::KEYWORD, NaikoKeyword::ENDIF);
                }
                break;
            case NaikoKeyword::WHILE:
                {
                    logger::Log::warning("%s", "STATEMENT-WHILE");
                    next();
                    processComparison();
                    match(TokenKind::KEYWORD, NaikoKeyword::REPEAT);
                    while (isTokenMatch(getCurrentToken(), TokenKind::KEYWORD, NaikoKeyword::ENDWHILE) == false)
                    {
                        statement();
                    }
                    match(TokenKind::KEYWORD, NaikoKeyword::ENDWHILE);
                    return;
                }
                break;
            default:
                AIKO_ASSERTF(false, "Not Implemented KEYWORD [%s]", current.text.data());
            }
        }
        else
        {
            AIKO_ASSERT(false, "Found an unknow keyword?");
        }

    }

}