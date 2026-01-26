#include <catch2/catch_test_macros.hpp>

#include <lexer.h>
#include <compiler_helper.h>

#include "test_types.h"
#include "tests_code.h"

TEST_CASE("Lexer returns END at EOF", "[LEXER]")
{
    const char* code = test_code_print_empty;
    INIT_LEXER
    REQUIRE(token.kind == TokenKind::END);
}

TEST_CASE("Lexer skips whitespaces", "[LEXER]" )
{
    const char* code = test_code_print_skip_whitespace;
    INIT_LEXER
    REQUIRE(token.text == "foo");
    REQUIRE(token.kind == TokenKind::SYMBOL);
}

TEST_CASE("Lexer sets correct symbols", "[LEXER]")
{

    const char* code = test_code_print_correct_symbol;

    std::vector<Expected> expecteds
    {
        KEYWORD(NaikoKeyword::LET, "LET" )
        SYMBOL("foobar")
        OP(NaikoOperation::EQUAL, "=")
        VALUE(NaikoType::DIGIT, "123" )
        END()
    };

    INIT_LEXER

    size_t current = 0;
    printToken(token);
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

TEST_CASE("Lexer basic let and set", "[LEXER]")
{

    const char* code = test_code_basic_let_and_set;

    std::vector<Expected> expecteds
    {
        KEYWORD(NaikoKeyword::LET, "LET" )
        SYMBOL("foobar")
        OP(NaikoOperation::EQUAL, "=")
        VALUE(NaikoType::DIGIT, "123" )
        KEYWORD(NaikoKeyword::SET, "SET" )
        SYMBOL("foobar")
        OP(NaikoOperation::EQUAL, "=")
        VALUE(NaikoType::DIGIT, "321" )
        END()
    };

    INIT_LEXER

    size_t current = 0;
    printToken(token);
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

TEST_CASE("Lexer print multiple tests", "[LEXER]" )
{
    const char* code = test_code_print_multiple_print;

    std::vector<Expected> expecteds
    {
        KEYWORD(NaikoKeyword::PRINT, "PRINT" )
        VALUE(NaikoType::STRING, "\"hello, world!\"" )
        KEYWORD(NaikoKeyword::PRINT, "PRINT" )
        VALUE(NaikoType::STRING, "\"second line\"" )
        KEYWORD(NaikoKeyword::PRINT, "PRINT" )
        VALUE(NaikoType::STRING, "\"and a third...\"" )
        END()
    };

    INIT_LEXER

    size_t current = 0;
    printToken(token);
    checkCurrentTokenIteration(expecteds, tokens, current++);
    while (token.kind != TokenKind::END)
    {
        token = lex.next();
        printToken(token);
        tokens.push_back(token);
        checkCurrentTokenIteration(expecteds, tokens, current++);
    }
}

TEST_CASE("Lexer print digit", "[LEXER]" )
{
    const char* code = test_code_print_digit;

    std::vector<Expected> expecteds
    {
        KEYWORD(NaikoKeyword::PRINT, "PRINT" )
        VALUE(NaikoType::DIGIT, "123" )
        END()
    };

    INIT_LEXER

    size_t current = 0;
    printToken(token);
    checkCurrentTokenIteration(expecteds, tokens, current++);
    while (token.kind != TokenKind::END)
    {
        token = lex.next();
        printToken(token);
        tokens.push_back(token);
        checkCurrentTokenIteration(expecteds, tokens, current++);
    }
}

TEST_CASE("Lexer if test", "[LEXER]" )
{
    const char* code = test_code_basic_if;

    std::vector<Expected> expecteds
    {
        KEYWORD(NaikoKeyword::IF, "IF" )
        VALUE(NaikoType::DIGIT, "10" )
        OP(NaikoOperation::GREATERTHAN, ">" )
        VALUE(NaikoType::DIGIT, "0" )
        KEYWORD(NaikoKeyword::THEN, "THEN" )
        KEYWORD(NaikoKeyword::PRINT, "PRINT" )
        VALUE(NaikoType::STRING, "yes!" )
        KEYWORD(NaikoKeyword::ENDIF, "ENDIF" )
        END()
    };

    INIT_LEXER

    size_t current = 0;
    printToken(token);
    checkCurrentTokenIteration(expecteds, tokens, current++);
    while (token.kind != TokenKind::END)
    {
        token = lex.next();
        printToken(token);
        tokens.push_back(token);
        checkCurrentTokenIteration(expecteds, tokens, current++);
    }
}

TEST_CASE("Lexer big program", "[LEXER]")
{
    aiko::string code = test_code_big_code;

        std::vector<Expected> expecteds =
        {
            KEYWORD(NaikoKeyword::PRINT, "PRINT" )
            VALUE(NaikoType::STRING, "\"How many fibonacci numbers do you want?\"" )

            KEYWORD(NaikoKeyword::LET, "LET" )
            SYMBOL("nums")
            OP(NaikoOperation::EQUAL, "=")
            VALUE(NaikoType::DIGIT, "123" )

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

            KEYWORD(NaikoKeyword::LET, "SET" )
            SYMBOL("a")
            OP(NaikoOperation::EQUAL, "=")
            SYMBOL("b")

            KEYWORD(NaikoKeyword::LET, "SET" )
            SYMBOL("b")
            OP(NaikoOperation::EQUAL, "=")
            SYMBOL("c")

            KEYWORD(NaikoKeyword::LET, "SET" )
            SYMBOL("nums")
            OP(NaikoOperation::EQUAL, "=")
            SYMBOL("nums")
            OP(NaikoOperation::SUBTRACT, "-")
            VALUE(NaikoType::DIGIT, "1" )

            KEYWORD(NaikoKeyword::PRINT, "PRINT" )
            SYMBOL("nums")

            KEYWORD(NaikoKeyword::ENDWHILE, "ENDWHILE" )
            END()
        };

        INIT_LEXER

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