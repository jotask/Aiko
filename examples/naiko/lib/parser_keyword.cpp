#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    NodePtr Parser::processKeyword()
    {
        Token& current = getCurrentToken();
        current.naiko = getKeywordKind(current.text);
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
                    const auto varToken = getCurrentToken();
                    match(TokenKind::IDENTIFIER);

                    NodePtr target = nullptr;
                    NodePtr expression = nullptr;

                    if (checkCurrent(TokenKind::SYMBOL, NaikoSymbol::OPEN_SQUARE) == true)
                    {
                        // array assignment
                        next();
                        NodePtr indexExpr = processExpression();
                        match(TokenKind::SYMBOL, NaikoSymbol::CLOSE_SQUARE);
                        target = std::make_unique<ArrayAccessNode>(varToken.text, std::move(indexExpr));
                    }
                    else
                    {
                        target = std::make_unique<VariableNode>(varToken.text);
                    }

                    if (checkCurrent(TokenKind::OPERATOR))
                    {
                        match(TokenKind::OPERATOR, NaikoOperation::EQUAL);
                        expression = processExpression();
                    }

                    return std::make_unique<LetNode>(std::move(target), std::move(expression));
                }
            case NaikoKeyword::SET:
                {
                    next();
                    const auto varToken = getCurrentToken();
                    match(TokenKind::IDENTIFIER);

                    NodePtr target = nullptr;
                    NodePtr expression = nullptr;

                    if (checkCurrent(TokenKind::SYMBOL, NaikoSymbol::OPEN_SQUARE) == true)
                    {
                        // array assigment
                        next();
                        NodePtr indexExpr = processExpression();
                        match(TokenKind::SYMBOL, NaikoSymbol::CLOSE_SQUARE);
                        target = std::make_unique<ArrayAccessNode>(varToken.text, std::move(indexExpr));
                    }
                    else
                    {
                        // Simple variable
                        target = std::make_unique<VariableNode>(varToken.text);
                    }

                    match(TokenKind::OPERATOR, NaikoOperation::EQUAL);
                    expression = processExpression();

                    return std::make_unique<SetNode>(std::move(target), std::move(expression));
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