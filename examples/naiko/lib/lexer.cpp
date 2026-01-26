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

        // Handle string
        if (m_code[m_cursor] == '"')
        {
            chopChar();

            token.kind = TokenKind::VALUE;
            token.naiko = NaikoType::STRING;

            token.position = m_cursor;

            auto isNotEndOfStr = [this]()
            {
                const auto shit = m_code[m_cursor];
                return m_code[m_cursor] != '"' ;
            };
            skipUntil(isNotEndOfStr);

            token.text = getStr( token.position, m_cursor);

            chopChar();

            return token;
        }

        skipUntilWhitespace();

        token.text = getStr( token.position, m_cursor);

        if (isPreprocessor(token.text) == true)
        {
            token.kind = TokenKind::PREPROCESSOR;
            return token;
        }

        if (isKeyword(token.text) == true)
        {
            token.kind = TokenKind::KEYWORD;
            token.naiko = getKeywordKind(token.text);
            return token;
        }

        if (isSymbol(token.text) == true)
        {
            token.kind = TokenKind::SYMBOL;
            return token;
        }

        if (isOperator(token.text) == true)
        {
            token.kind = TokenKind::OPERATOR;
            token.naiko = getOperationKind(token.text);
            return token;
        }

        if (isDigit(token.text) == true)
        {
            token.kind = TokenKind::VALUE;
            token.naiko = NaikoType::DIGIT;
            return token;
        }

        return token;
    }

    void Lexer::readInputFiles(std::vector<string> inputFiles)
    {
        AIKO_ASSERT(inputFiles.size() == 1, "We only support one single file for now");
        m_code = aiko::files::readFileContent(inputFiles.front().c_str());
    }

    void Lexer::chopChar(size_t many)
    {
        for (size_t i = 0 ; i < many ; ++i)
        {
            const char value = m_code[m_cursor];
            m_cursor++;
            if (value == NEW_LINE)
            {
                m_line++;
            }
        }
    }

    void Lexer::skipUntil(FntUntilCompare fnt)
    {
        while (m_cursor < m_code.size() && fnt())
        {
            chopChar();
        }
    }

    void Lexer::skipUntilWhitespace()
    {
        auto isNotWhiteSpace = [&](){ return static_cast<bool>(std::isspace(static_cast<unsigned char>(m_code[m_cursor]))) == false; };
        skipUntil(isNotWhiteSpace);
    }

    void Lexer::skipWhitespace()
    {
        auto isNotWhiteSpace = [&](){ return static_cast<bool>(std::isspace(static_cast<unsigned char>(m_code[m_cursor]))) == true; };
        skipUntil(isNotWhiteSpace);
    }

    bool Lexer::isFirstCharacter(string str, unsigned char c) const
    {
        const auto front = static_cast<unsigned char>(str.front());
        return front == c;
    }

    bool Lexer::isPreprocessor(string str) const
    {
        return isFirstCharacter(str, '#');
    }

    bool Lexer::isKeyword(string str) const
    {
        const auto values = magic_enum::enum_names<NaikoKeyword>();
        return std::any_of(values.begin(), values.end(), [str](std::string_view other){ return str == other; });
    }

    bool Lexer::isSymbol(string str) const
    {
        const auto isSymbolStart = [&](const char c) -> bool
        {
            return static_cast<bool>(std::isalpha(static_cast<unsigned char>(c)));
        };

        const auto isSymbol = [&](const char c) -> bool
        {
            return static_cast<bool>(std::isalnum(static_cast<unsigned char>(c)));
        };

        if (isSymbolStart(str.front()) == false)
        {
            return false;
        }

        for (size_t i = 0; i < str.size(); ++i)
        {
            if (isSymbol(str.at(i)) == false)
            {
                return false;
            }
        }
        return true;
    }

    bool Lexer::isOperator(string str) const
    {
        static const std::vector<char> operators = {'+', '-', '*', '/', '=', '<', '>'};
        if ( str.size() != 1 ) return false;
        const unsigned char front = static_cast<unsigned char>(str.front());
        return std::any_of(operators.begin(), operators.end(), [front](const char other){ return other == front; } );
    }

    bool Lexer::isDigit(string str) const
    {
        for (size_t i = 0 ; i < str.size(); i++)
        {
            if ( std::isdigit(str.at(i)) == false )
            {
                return false;
            }
        }
        return true;
    }

    string Lexer::getStr(size_t start, size_t end) const
    {
        return m_code.substr(start, end - start);
    }
}
