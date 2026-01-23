#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{
    Parser::Parser(const Tokenization tokens)
        : m_tokens(tokens)
    {

    }

    void Parser::program()
    {
        setup();
        logger::Log::warning("%s", "PROGRAM");
        while (checkCurrent(TokenKind::END) == false)
        {
            statement();
        }
    }

    void Parser::setup()
    {
        m_current = 0;
    }

    void Parser::next()
    {
        m_current++;
    }

    void Parser::match(TokenKind kind)
    {
        if (checkCurrent(kind) == false)
        {
            const auto expectedName = magic_enum::enum_name<TokenKind>(kind);
            const auto gotName = magic_enum::enum_name<TokenKind>(m_tokens[m_current].kind);
            logger::Log::critical("Expected %s but got %s", expectedName.data(), gotName.data());
            std::exit(-1);
        }
        next();
    }

    void Parser::match(TokenKind kind, Naiko naiko)
    {

        if (checkCurrent(kind) == false)
        {
            const auto expectedName = magic_enum::enum_name<TokenKind>(kind);
            const auto gotName = magic_enum::enum_name<TokenKind>(m_tokens[m_current].kind);
            logger::Log::critical("Expected %s but got %s", expectedName.data(), gotName.data());
            std::exit(-1);
        }

        const auto current = getCurrentToken();
        bool matched = false;

        // Use std::visit to handle the variant generically
        std::visit([&](auto&& expected)
        {

            using T = std::decay_t<decltype(expected)>;

            // Only match if T is not monostate
            if constexpr (!std::is_same_v<T, std::monostate>)
            {
                matched = isTokenMatch(current, kind, expected);
            }
        }, naiko);

        if (matched == false)
        {
            const auto expectedName = magic_enum::enum_name<TokenKind>(kind);
            const auto gotName = magic_enum::enum_name<TokenKind>(current.kind);
            logger::Log::critical("Token mismatch: expected %s with correct value but got %s", expectedName.data(), gotName.data());
            AIKO_DEBUG_BREAK
            std::exit(-1);
        }
        next();
    }

    bool Parser::checkCurrent(TokenKind kind)
    {
        return getCurrentToken().kind == kind;
    }

    bool Parser::checkNext(TokenKind kind)
    {
        return getNextToken().kind == kind;
    }

    Token Parser::getCurrentToken() const
    {
        AIKO_ASSERT( m_current < m_tokens.size(), "OOB");
        return m_tokens[m_current];
    }

    Token Parser::getNextToken() const
    {
        AIKO_ASSERT( m_current + 1 < m_tokens.size(), "OOB");
        return m_tokens[m_current + 1];
    }

}
