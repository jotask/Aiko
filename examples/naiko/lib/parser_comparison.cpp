#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    void Parser::processComparison()
    {
        logger::Log::warning("%s", "COMPARISON");
        processExpression();
        if (isComparisonOperator() == true)
        {
            next();
            processExpression();
        }

        while (isComparisonOperator() == true)
        {
            next();
            processExpression();
        }
    }

    void Parser::processExpression()
    {
        logger::Log::warning("%s", "EXPRESSION");
        processTerm();
        auto token = getCurrentToken();
        while (checkCurrent(TokenKind::OPERATOR, NaikoOperation::ADD) || checkCurrent(TokenKind::OPERATOR, NaikoOperation::SUBTRACT) )
        {
            next();
            processTerm();
        }
    }

    void Parser::processTerm()
    {
        logger::Log::warning("%s", "TERM");
        processUnary();
        while (checkCurrent(TokenKind::OPERATOR, NaikoOperation::MULTIPLY) || checkCurrent(TokenKind::OPERATOR, NaikoOperation::DIVIDE))
        {
            next();
            processUnary();
        }
    }

    void Parser::processUnary()
    {
        logger::Log::warning("%s", "UNARY");
        if (checkCurrent(TokenKind::OPERATOR, NaikoOperation::ADD) || checkCurrent(TokenKind::OPERATOR, NaikoOperation::SUBTRACT))
        {
            next();
        }
        processPrimary();
    }

    void Parser::processPrimary()
    {
        logger::Log::warning("%s -> %s", "PRIMARY", getCurrentToken().text.data());
        const auto curr = getCurrentToken();
        if (checkCurrent(TokenKind::VALUE, NaikoType::DIGIT))
        {
            next();
        }
        else if (checkCurrent(TokenKind::SYMBOL))
        {
            next();
        }
        else
        {
            AIKO_ASSERTF(false, "Unexpected token at %s", getCurrentToken().text.data());
        }
    }

    bool Parser::isComparisonOperator() const
    {
        const auto current = getCurrentToken();
        return current.kind == TokenKind::OPERATOR;
    }

}
