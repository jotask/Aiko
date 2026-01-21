#include <catch2/catch_test_macros.hpp>

#include <lexer.h>
#include <compiler_helper.h>

using namespace aiko::naiko;

struct Expected
{
    TokenKind kind;
    Naiko naiko;
    aiko::string text;
};

#define OP(naiko, text)         { TokenKind::OPERATOR,  naiko,  text },
#define KEYWORD(naiko, text)    { TokenKind::KEYWORD,   naiko,  text },
#define VALUE(naiko, text)      { TokenKind::VALUE,     naiko,  text },
#define SYMBOL(text)            { TokenKind::SYMBOL,    {},     text },
#define END()                   { TokenKind::END,       {},     {} },


static void checkCurrentTokenIteration(std::vector<Expected> expecteds, std::vector<Token> tokens, size_t idx)
{
    const auto exp = expecteds[idx];
    const auto cur = tokens[idx];
    REQUIRE(exp.kind == cur.kind);

    const bool currContainsNaiko = std::holds_alternative<std::monostate>(cur.naiko);
    const bool expcContainsNaiko = std::holds_alternative<std::monostate>(exp.naiko);

    REQUIRE(currContainsNaiko == expcContainsNaiko);

    if (currContainsNaiko == false)
    {
        return;
    }

    switch (cur.kind)
    {
        case TokenKind::KEYWORD:
            {
                const auto op = getKeywordKind(cur.text);
                REQUIRE(op != NaikoKeyword::INVALID);
                auto* expNaiko = std::get_if<NaikoKeyword>(&exp.naiko);
                REQUIRE(expNaiko != nullptr);
                REQUIRE(op == *expNaiko);
            }
            break;
        case TokenKind::OPERATOR:
            {
                const auto op = getOperationKind(cur.text);
                REQUIRE(op != NaikoOperation::INVALID);
                auto* expNaiko = std::get_if<NaikoOperation>(&exp.naiko);
                REQUIRE(expNaiko != nullptr);
                REQUIRE(op == *expNaiko);
            }
            break;
        case TokenKind::VALUE:
            {
                const auto op = getTypeKind(cur.text);
                REQUIRE(op != NaikoType::INVALID);
                auto* expNaiko = std::get_if<NaikoType>(&exp.naiko);
                REQUIRE(expNaiko != nullptr);
                REQUIRE(op == *expNaiko);
            }
            break;
        default:
            break;
    }

}

TEST_CASE("Lexer skips whitespaces", "[LEXER]" )
{
    auto lex = Lexer("  foo");
    auto tok = lex.next();
    REQUIRE(tok.text == "foo");
    REQUIRE(tok.kind == TokenKind::SYMBOL);
}

TEST_CASE("Lexer returns END at EOF", "[LEXER]")
{
    Lexer lex("");
    auto tok = lex.next();
    REQUIRE(tok.kind == TokenKind::END);
}


TEST_CASE("Lexer sets correct symbols", "[LEXER]")
{
    Lexer lex("LET foobar = 123");
    std::vector<Token> tokens;

    {
        Token token = lex.next();
        tokens.push_back(token);
        while (token.kind != TokenKind::END)
        {
            token = lex.next();
            tokens.push_back(token);
        }
    }

    std::vector<Expected> expectations
    {
        KEYWORD(NaikoKeyword::LET, "LET" )
        SYMBOL("foobar")
        OP(NaikoOperation::EQUAL, "=")
        VALUE(NaikoType::DIGIT, "123" )
        END()
    };

    REQUIRE(tokens.size() == expectations.size());

    for (size_t i = 0 ; i < tokens.size(); i++)
    {
        checkCurrentTokenIteration(expectations, tokens, i);
    }

}

TEST_CASE("Lexer big program", "[LEXER]")
{
    aiko::string code =
        "PRINT \"How many fibonacci numbers do you want?\"\n"
        "INPUT nums\n"
        "LET a = 0\n"
        "LET b = 1\n"
        "WHILE nums > 0 REPEAT\n"
        "    PRINT a\n"
        "    LET c = a + b\n"
        "    LET a = b\n"
        "    LET b = c\n"
        "    LET nums = nums - 1\n"
        "ENDWHILE";

    Lexer lex(code);

    {

        Tokenization tokens;

        std::vector<Expected> expecteds =
        {
            KEYWORD(NaikoKeyword::PRINT, "PRINT" )
            VALUE(NaikoType::STRING, "\"How many fibonacci numbers do you want?\"" )

            KEYWORD(NaikoKeyword::INPUT, "INPUT" )
            SYMBOL("nums")

            KEYWORD(NaikoKeyword::LET, "LET" )
            SYMBOL("a")
            OP(NaikoOperation::EQUAL, "=")
            VALUE(NaikoType::DIGIT, "0" )

            KEYWORD(NaikoKeyword::LET, "LET" )
            SYMBOL("b")
            OP(NaikoOperation::EQUAL, "=")
            VALUE(NaikoType::DIGIT, "1" )


            KEYWORD(NaikoKeyword::WHILE, "WHILE" )
            SYMBOL("nums")
            OP(NaikoOperation::GREATERTHAN, ">")
            VALUE(NaikoType::DIGIT, "0" )
            KEYWORD(NaikoKeyword::REPEAT, "REPEAT" )

            KEYWORD(NaikoKeyword::PRINT, "PRINT" )
            SYMBOL("a")

            KEYWORD(NaikoKeyword::LET, "LET" )
            SYMBOL("c")
            OP(NaikoOperation::EQUAL, "=")
            SYMBOL("a")
            OP(NaikoOperation::ADD, "+")
            SYMBOL("b")

            KEYWORD(NaikoKeyword::LET, "LET" )
            SYMBOL("a")
            OP(NaikoOperation::EQUAL, "=")
            SYMBOL("b")

            KEYWORD(NaikoKeyword::LET, "LET" )
            SYMBOL("b")
            OP(NaikoOperation::EQUAL, "=")
            SYMBOL("c")

            KEYWORD(NaikoKeyword::LET, "LET" )
            SYMBOL("nums")
            OP(NaikoOperation::EQUAL, "=")
            SYMBOL("nums")
            OP(NaikoOperation::SUBTRACT, "-")
            VALUE(NaikoType::DIGIT, "1" )

            KEYWORD(NaikoKeyword::ENDWHILE, "ENDWHILE" )
            END()
        };

        Token token = lex.next();
        tokens.push_back(token);
        size_t current = 0;
        checkCurrentTokenIteration(expecteds, tokens, current++);
        while (token.kind != TokenKind::END)
        {
            token = lex.next();
            printToken(token);
            tokens.push_back(token);
            checkCurrentTokenIteration(expecteds, tokens, current++);
        }

        REQUIRE(token.kind == TokenKind::END);
        REQUIRE(tokens.size() == expecteds.size());
    }


}