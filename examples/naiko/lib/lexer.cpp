#include "lexer.h"

#include "compiler_types.h"

#include <magic_enum/magic_enum.hpp>

#include <core/file.h>
#include <logger/logger.h>

namespace aiko::naiko
{
    Lexer::Lexer(string code)
        : m_code(code)
        , m_cursor(0)
        , m_line(0)
    {
    }

    Lexer::Lexer(const CompilerOptions opts)
        : m_code()
        , m_cursor(0)
        , m_line(0)
    {
        readInputFiles(opts.inputFiles);
    }

    Token Lexer::next()
    {

        // Trim left
        skipWhitespace();

        Token token = {
            .kind = TokenKind::INVALID,
            .position = m_cursor,
            .line = m_line
        };

        if (m_cursor >= m_code.size())
        {
            token.kind = TokenKind::END;
            return token;
        }

        const char c = m_code[m_cursor];

        // Handle string
        if (c == '"')
        {
            chopChar();

            token.kind = TokenKind::VALUE;
            token.naiko = NaikoType::STRING;
            token.position = m_cursor;

            auto isNotEndOfStr = [this]()
            {
                return m_cursor < m_code.size() && m_code[m_cursor] != '"' ;
            };
            skipUntil(isNotEndOfStr);

            token.text = getStr( token.position, m_cursor);

            chopChar();

            return token;
        }

        // Identifier or keyword
        if (isFunction(std::isalpha, c) == true)
        {
            const size_t start = m_cursor;
            while (m_cursor < m_code.size())
            {
                char cc = m_code[m_cursor];
                if (isFunction(std::isalnum, cc) == false && cc != '_')
                {
                    break;
                }
                chopChar();
            }

            token.text = getStr(start, m_cursor);

            if (isKeyword(token.text))
            {
                token.kind = TokenKind::KEYWORD;
            }
            else
            {
               token.kind = TokenKind::IDENTIFIER;
            }

            return token;
        }

        // number literal
        if (isFunction(std::isdigit, c) == true)
        {
            const size_t start = m_cursor;
            while (m_cursor < m_code.size() && isFunction(std::isdigit, m_code[m_cursor]) == true)
            {
                chopChar();
            }
            token.text = getStr(start, m_cursor);
            token.kind = TokenKind::VALUE;
            token.naiko = NaikoType::DIGIT;
            return token;
        }

        // Symbol (single character)
        if (isSymbol(c) == true)
        {
            token.text = std::string(1, c);
            token.kind = TokenKind::SYMBOL;
            chopChar();
            return token;
        }

        // Operator (single character)
        if (isOperator(c) == true)
        {
            token.text = std::string(1, c);
            token.kind = TokenKind::OPERATOR;
            chopChar();
            return token;
        }

        // Unknown character
        logger::Log::error("Unknown character %c at line %zu", c, m_line);
        chopChar();
        return token;
    }

    void Lexer::readInputFiles(std::vector<string> inputFiles)
    {
        AIKO_ASSERT(inputFiles.size() == 1, "We only support one single file for now");
        m_code = aiko::files::readFileContent(inputFiles.front().c_str());
    }

    void Lexer::chopChar()
    {
        const char value = m_code[m_cursor];
        m_cursor++;
        if (value == NEW_LINE)
        {
            m_line++;
        }
    }

    void Lexer::skipUntil(FntUntilCompare fnt)
    {
        while (m_cursor < m_code.size() && fnt())
        {
            chopChar();
        }
    }

    void Lexer::skipWhitespace()
    {
        auto isWhiteSpace = [&](){ return static_cast<bool>(std::isspace(static_cast<unsigned char>(m_code[m_cursor]))) == true; };
        skipUntil(isWhiteSpace);
    }

    bool Lexer::isKeyword(string str) const
    {
        const auto values = magic_enum::enum_names<NaikoKeyword>();
        return std::any_of(values.begin(), values.end(), [str](std::string_view other){ return str == other; });
    }

    bool Lexer::isSymbol(const char c) const
    {
        static const std::vector<char> operators = {'(', ')', '{', '}', '[', ']'};
        const unsigned char front = static_cast<unsigned char>(c);
        return std::any_of(operators.begin(), operators.end(), [front](const char other){ return other == front; } );
    }

    bool Lexer::isOperator(const char c) const
    {
        static const std::vector<char> operators = {'+', '-', '*', '/', '=', '<', '>'};
        const unsigned char front = static_cast<unsigned char>(c);
        return std::any_of(operators.begin(), operators.end(), [front](const char other){ return other == front; } );
    }

    bool Lexer::isFunction(IsFnt fnt, char c)
    {
        return fnt(static_cast<unsigned char>(c)) != 0;
    }

    string Lexer::getStr(size_t start, size_t end) const
    {
        return m_code.substr(start, end - start);
    }
}
