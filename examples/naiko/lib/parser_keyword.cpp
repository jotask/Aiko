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
                    next();
                    return std::make_unique<PrintNode>(processExpression());
                }
            case NaikoKeyword::LET:
                {
                    next();
                    match(TokenKind::SYMBOL);
                    const auto name = getCurrentToken().text;
                    match(TokenKind::OPERATOR, NaikoOperation::EQUAL);
                    return std::make_unique<LetNode>(name, processExpression());
                }
            case NaikoKeyword::IF:
                {
                    next();
                    auto condition = processComparison();
                    match(TokenKind::KEYWORD, NaikoKeyword::THEN);
                    std::vector<NodePtr> body;
                    while (isTokenMatch(getCurrentToken(), TokenKind::KEYWORD, NaikoKeyword::ENDIF) == false)
                    {
                        auto stmt = processStatement();
                        body.push_back(std::move(stmt));
                    }
                    match(TokenKind::KEYWORD, NaikoKeyword::ENDIF);
                    return std::make_unique<IfNode>(std::move(condition), std::move(body));
                }
            case NaikoKeyword::WHILE:
                {
                    next();
                    auto condition = processComparison();
                    match(TokenKind::KEYWORD, NaikoKeyword::REPEAT);
                    std::vector<NodePtr> body;
                    while (isTokenMatch(getCurrentToken(), TokenKind::KEYWORD, NaikoKeyword::ENDWHILE) == false)
                    {
                        auto stmt = processStatement();
                        body.push_back(std::move(stmt));
                    }
                    match(TokenKind::KEYWORD, NaikoKeyword::ENDWHILE);
                    return std::make_unique<WhileNode>(std::move(condition), std::move(body));
                }
            default:
                AIKO_ASSERTF(false, "Not Implemented KEYWORD [%s]", current.text.data());
            }
        }
        AIKO_ASSERTF(false, "Found an unknow keyword? or KEYWORD not implemented [%s]", current.text.data());
    }

}