#include "parser.h"

#include <logger/logger.h>

namespace aiko::naiko
{

    NodePtr Parser::processRelational()
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
        return processPostfix();
    }

    NodePtr Parser::processPostfix()
    {
        NodePtr node = processPrimary();

        while (true)
        {

            // function call
            if (checkCurrent(TokenKind::SYMBOL, NaikoSymbol::OPEN_PARENT))
            {
                next();
                std::vector<NodePtr> arguments;
                while (checkCurrent(TokenKind::SYMBOL, NaikoSymbol::CLOSE_PARENT) == false)
                {
                    auto stmt = processExpression();
                    arguments.push_back(std::move(stmt));
                    if (checkCurrent(TokenKind::SYMBOL, NaikoSymbol::COMMA))
                    {
                        next();
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                match(TokenKind::SYMBOL, NaikoSymbol::CLOSE_PARENT);
                auto identifier = dynamic_cast<VariableNode*>(node.get());
                AIKO_ASSERT(identifier != nullptr, "Call target must be identifier");
                node = std::make_unique<CallExpressionNode>(identifier->name, std::move(arguments));
                continue;
            }

            // Array access
            if (checkCurrent(TokenKind::SYMBOL, NaikoSymbol::OPEN_SQUARE))
            {
                next();
                auto index = processExpression();
                match(TokenKind::SYMBOL, NaikoSymbol::CLOSE_SQUARE);
                auto identifier = dynamic_cast<VariableNode*>(node.get());
                AIKO_ASSERT(identifier != nullptr, "Array access target must be identifier");
                node = std::make_unique<ArrayAccessNode>(identifier->name, std::move(index));
                continue;
            }
            break;
        }

        return node;
    }

    NodePtr Parser::processPrimary()
    {
        const auto curr = getCurrentToken();
        if (checkCurrent(TokenKind::VALUE, NaikoType::INT))
        {
            next();
            return std::make_unique<NumberNode>(std::stoi(curr.text));
        }
        if (checkCurrent(TokenKind::VALUE, NaikoType::CHAR))
        {
            next();
            if (curr.text.size() != 1)
            {
                logger::Log::error("Expected char but got string!");
                std::exit(-1);
            }
            char c = curr.text.at(0);
            return std::make_unique<CharNode>(c);
        }
        if (checkCurrent(TokenKind::VALUE, NaikoType::STRING))
        {
            next();
            return std::make_unique<StringNode>(curr.text);
        }

        if (checkCurrent(TokenKind::IDENTIFIER))
        {
            auto name = curr.text;
            next();
            return std::make_unique<VariableNode>(curr.text);
        }
        if (checkCurrent(TokenKind::SYMBOL, NaikoSymbol::OPEN_PARENT))
        {
            next();
            auto expr = processExpression();
            match(TokenKind::SYMBOL, NaikoSymbol::CLOSE_PARENT);
            return expr;
        }
        AIKO_ASSERTF(false, "Unexpected token at %s", getCurrentToken().text.data());
    }

    bool Parser::isComparisonOperator() const
    {
        if (getCurrentToken().kind != TokenKind::OPERATOR)
        {
            return false;
        }

        auto op = std::get<NaikoOperation>(getCurrentToken().naiko);
        switch (op)
        {
            case NaikoOperation::EQUALEQUAL:
            case NaikoOperation::NOTEQUAL:
            case NaikoOperation::LESSTHAN:
            case NaikoOperation::LESSTHANEQUAL:
            case NaikoOperation::GREATERTHAN:
            case NaikoOperation::GREATERTHANEQUAL:
                return true;
            default:
                return false;
        }
    }

}
