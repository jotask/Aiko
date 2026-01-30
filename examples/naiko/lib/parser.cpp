#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

#include "token_naiko_helper.h"

namespace aiko::naiko
{
    Parser::Parser(const Tokenization tokens)
        : m_tokens(tokens)
    {

    }

    NodeUPtr<ProgramNode> Parser::program()
    {
        setup();
        NodeUPtr<ProgramNode> program = std::make_unique<ProgramNode>();
        while (checkCurrent(TokenKind::END) == false)
        {
            auto stmnt = processStatement();
            if (stmnt != nullptr)
            {
                program->statements.push_back(std::move(stmnt));
            }
        }
        return std::move(program);
    }

    void Parser::setup()
    {
        m_current = 0;
        for (auto& token : m_tokens)
        {
            // lazy resolution
            helper::resolveNaiko(token);
        }
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

    const Token& Parser::getCurrentToken() const
    {
        AIKO_ASSERT( m_current < m_tokens.size(), "OOB");
        return m_tokens[m_current];
    }

    Token& Parser::getCurrentToken()
    {
        AIKO_ASSERT( m_current < m_tokens.size(), "OOB");
        return m_tokens[m_current];
    }



}
