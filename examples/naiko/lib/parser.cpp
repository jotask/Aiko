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
        switch (current.kind)
        {
            case TokenKind::KEYWORD:
                {
                    const NaikoKeyword* keyword = std::get_if<NaikoKeyword>(&current.naiko);
                    if (keyword != nullptr)
                    {
                        logger::Log::critical("Expected NaikoKeyword for keyword but got something else");
                        std::exit(-1);
                    }
                }
            break;
            default:
            AIKO_ASSERT(false, "Unknow")
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

    void Parser::statement()
    {
        const auto current = getCurrentToken();
        switch (current.kind)
        {
            case TokenKind::END:
                {
                    logger::Log::warning("END");
                    return;
                } break;
            case TokenKind::KEYWORD:
                {
                    processKeyword();
                } break;
            default:
                AIKO_ASSERT(false, "unknow");
        }
    }

}
