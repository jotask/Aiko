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
            case NaikoKeyword::CONST:
                {

                    bool isContant = *keyword == NaikoKeyword::CONST;

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
                        NodePtr base = std::make_unique<VariableNode>(varToken.text);
                        target = std::make_unique<ArrayAccessNode>(std::move(base), std::move(indexExpr));
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

                    return std::make_unique<LetNode>(std::move(target), std::move(expression), isContant);
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
                        NodePtr base = std::make_unique<VariableNode>(varToken.text);
                        target = std::make_unique<ArrayAccessNode>(std::move(base), std::move(indexExpr));
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
                    auto condition = processRelational();
                    match(TokenKind::KEYWORD, NaikoKeyword::THEN);
                    std::vector<NodePtr> body;
                    while (isTokenMatch(getCurrentToken(), TokenKind::KEYWORD, NaikoKeyword::END) == false)
                    {
                        auto stmt = processStatement();
                        body.push_back(std::move(stmt));
                    }
                    match(TokenKind::KEYWORD, NaikoKeyword::END);
                    return std::make_unique<IfNode>(std::move(condition), std::move(body));
                }
            case NaikoKeyword::WHILE:
                {
                    next();
                    auto condition = processRelational();
                    match(TokenKind::KEYWORD, NaikoKeyword::REPEAT);
                    std::vector<NodePtr> body;
                    while (isTokenMatch(getCurrentToken(), TokenKind::KEYWORD, NaikoKeyword::END) == false)
                    {
                        auto stmt = processStatement();
                        body.push_back(std::move(stmt));
                    }
                    match(TokenKind::KEYWORD, NaikoKeyword::END);
                    return std::make_unique<WhileNode>(std::move(condition), std::move(body));
                }
            case NaikoKeyword::RETURN:
                {
                    next();
                    NodePtr returnExpr = processExpression();
                    return std::make_unique<ReturnNode>(std::move(returnExpr));
                }
            case NaikoKeyword::FUNCTION:
                {
                    next();
                    auto functionName = getCurrentToken();
                    match(TokenKind::IDENTIFIER);
                    // Do we have parameters?
                    std::vector<FunctionNode::Parameter> parameters;
                    if (checkCurrent(TokenKind::SYMBOL, NaikoSymbol::COLON) ==  true)
                    {
                        match(TokenKind::SYMBOL, NaikoSymbol::COLON);
                        do
                        {
                            if (checkCurrent(TokenKind::IDENTIFIER) == false)
                            {
                                logger::Log::error("Expected parameter name after ':'");
                                std::exit(-1);
                            }
                            auto currentToken = getCurrentToken();
                            parameters.push_back({.name = currentToken.text, .type = NaikoType::UNKNOWN});
                            next();
                            if (checkCurrent(TokenKind::SYMBOL, NaikoSymbol::COMMA))
                            {
                                next(); // consume comma
                            }
                            else
                            {
                                break; // no more parameters
                            }
                        }
                        while (checkCurrent(TokenKind::IDENTIFIER));

                    }

                    std::vector<NodePtr> body;
                    while (isTokenMatch(getCurrentToken(), TokenKind::KEYWORD, NaikoKeyword::END) == false)
                    {
                        NodePtr stmt = processStatement();
                        body.push_back(std::move(stmt));
                    }

                    match(TokenKind::KEYWORD, NaikoKeyword::END);

                    return std::make_unique<FunctionNode>( functionName.text, parameters, std::move(body));

                }
            default:
                AIKO_ASSERTF(false, "Not Implemented KEYWORD [%s]", current.text.data());
            }
        }
        AIKO_ASSERTF(false, "Found an unknow keyword? or KEYWORD not implemented [%s]", current.text.data());
    }

}