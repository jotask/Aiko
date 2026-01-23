#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    NodePtr Parser::processComparison()
    {
        auto left = processExpression();
        if (isComparisonOperator() == true)
        {
            const auto opToken = getCurrentToken();
            next();
            auto right = processExpression();
            left = std::make_unique<BinaryOperationNode>(std::move(left), std::get<NaikoOperation>(opToken.naiko), std::move(right));
        }
        return left;
    }

    NodePtr Parser::processExpression()
    {
        auto left = processTerm();
        auto token = getCurrentToken();
        while (checkCurrent(TokenKind::OPERATOR, NaikoOperation::ADD) || checkCurrent(TokenKind::OPERATOR, NaikoOperation::SUBTRACT) )
        {
            const auto opToken = getCurrentToken();
            next();
            auto right = processTerm();
            left = std::make_unique<BinaryOperationNode>(std::move(left), std::get<NaikoOperation>(opToken.naiko),std::move(right));
        }
        return left;
    }

    NodePtr Parser::processTerm()
    {
        auto left = processUnary();
        while (checkCurrent(TokenKind::OPERATOR, NaikoOperation::MULTIPLY) || checkCurrent(TokenKind::OPERATOR, NaikoOperation::DIVIDE))
        {
            const auto opToken = getCurrentToken();
            next();
            auto right = processUnary();
            left = std::make_unique<BinaryOperationNode>(std::move(left), std::get<NaikoOperation>(opToken.naiko),std::move(right));
        }
        return left;
    }

    NodePtr Parser::processUnary()
    {
        if (checkCurrent(TokenKind::OPERATOR, NaikoOperation::ADD) || checkCurrent(TokenKind::OPERATOR, NaikoOperation::SUBTRACT))
        {
            const auto opToken = getCurrentToken();
            next();
            auto operand = processUnary();
            return std::make_unique<UnaryOperationNode>(std::get<NaikoOperation>(opToken.naiko), std::move(operand));
        }
        return processPrimary();
    }

    NodePtr Parser::processPrimary()
    {
        const auto curr = getCurrentToken();
        if (checkCurrent(TokenKind::VALUE, NaikoType::DIGIT))
        {
            next();
            return std::make_unique<NumberNode>(std::stoi(curr.text));
        }
        if (checkCurrent(TokenKind::VALUE, NaikoType::STRING))
        {
            next();
            return std::make_unique<StringNode>(curr.text);
        }
        if (checkCurrent(TokenKind::SYMBOL))
        {
            next();
            return std::make_unique<VariableNode>(curr.text);
        }
        if (checkCurrent(TokenKind::KEYWORD, NaikoKeyword::OPEN_PARENT))
        {
            next();
            auto expr = processExpression();
            match(TokenKind::KEYWORD, NaikoKeyword::CLOSE_PARENT);
            return expr;
        }
        AIKO_ASSERTF(false, "Unexpected token at %s", getCurrentToken().text.data());
    }

    bool Parser::isComparisonOperator() const
    {
        const auto current = getCurrentToken();
        return current.kind == TokenKind::OPERATOR;
    }

}
