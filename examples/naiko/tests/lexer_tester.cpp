#include <catch2/catch_test_macros.hpp>

#include <lexer.h>
#include <compiler_helper.h>

using namespace aiko::naiko;

struct Expected
{
    TokenKind kind;
    aiko::string text;
};

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
        { .kind = TokenKind::KEYWORD,   .text = "LET" },
        { .kind = TokenKind::SYMBOL,    .text ="foobar" },
        { .kind = TokenKind::OPERATOR,  .text ="=" },
        { .kind = TokenKind::DIGIT,     .text ="123" },
        { .kind = TokenKind::END,       .text ="" },
    };

    REQUIRE(tokens.size() == expectations.size());

    for (size_t i = 0 ; i < tokens.size(); i++)
    {
        const Token curr= tokens[i];
        const Expected expe   = expectations[i];
        REQUIRE(curr.kind == expe.kind);
        REQUIRE(curr.text == expe.text);
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

        std::vector<Token> tokens;
        std::vector<Expected> expecteds =
        {
            { .kind = TokenKind::KEYWORD,   .text = "PRINT" },
            { .kind = TokenKind::STRING,    .text ="\"How many fibonacci numbers do you want?\"" },

            { .kind = TokenKind::KEYWORD,  .text ="INPUT" },
            { .kind = TokenKind::SYMBOL,     .text ="nums" },

            { .kind = TokenKind::KEYWORD,       .text ="LET" },
            { .kind = TokenKind::SYMBOL,       .text ="a" },
            { .kind = TokenKind::OPERATOR,       .text ="=" },
            { .kind = TokenKind::DIGIT,       .text ="0" },

            { .kind = TokenKind::KEYWORD,       .text ="LET" },
            { .kind = TokenKind::SYMBOL,       .text ="b" },
            { .kind = TokenKind::OPERATOR,       .text ="=" },
            { .kind = TokenKind::DIGIT,       .text ="1" },

            { .kind = TokenKind::KEYWORD,       .text ="WHILE" },
            { .kind = TokenKind::SYMBOL,       .text ="nums" },
            { .kind = TokenKind::OPERATOR,       .text =">" },
            { .kind = TokenKind::DIGIT,       .text ="0" },
            { .kind = TokenKind::KEYWORD,       .text ="REPEAT" },

            { .kind = TokenKind::KEYWORD,       .text ="PRINT" },
            { .kind = TokenKind::SYMBOL,       .text ="a" },

            { .kind = TokenKind::KEYWORD,       .text ="LET" },
            { .kind = TokenKind::SYMBOL,       .text ="c" },
            { .kind = TokenKind::OPERATOR,       .text ="=" },
            { .kind = TokenKind::SYMBOL,       .text ="a" },
            { .kind = TokenKind::OPERATOR,       .text ="+" },
            { .kind = TokenKind::SYMBOL,       .text ="b" },

            { .kind = TokenKind::KEYWORD,       .text ="LET" },
            { .kind = TokenKind::SYMBOL,       .text ="a" },
            { .kind = TokenKind::OPERATOR,       .text ="=" },
            { .kind = TokenKind::SYMBOL,       .text ="b" },

            { .kind = TokenKind::KEYWORD,       .text ="LET" },
            { .kind = TokenKind::SYMBOL,       .text ="b" },
            { .kind = TokenKind::OPERATOR,       .text ="=" },
            { .kind = TokenKind::SYMBOL,       .text ="c" },

            { .kind = TokenKind::KEYWORD,       .text ="LET" },
            { .kind = TokenKind::SYMBOL,       .text ="nums" },
            { .kind = TokenKind::OPERATOR,       .text ="=" },
            { .kind = TokenKind::SYMBOL,       .text ="nums" },
            { .kind = TokenKind::OPERATOR,       .text ="-" },
            { .kind = TokenKind::DIGIT,       .text ="1" },

            { .kind = TokenKind::KEYWORD,       .text ="ENDWHILE" },
            { .kind = TokenKind::END,       .text ="" },

        };

        Token token = lex.next();
        tokens.push_back(token);
        while (token.kind != TokenKind::END)
        {
            token = lex.next();
            aiko::naiko::printToken(token);
            tokens.push_back(token);
            REQUIRE(token.kind != TokenKind::INVALID);
        }

        REQUIRE(token.kind == TokenKind::END);
        REQUIRE(tokens.size() == expecteds.size());
    }


}