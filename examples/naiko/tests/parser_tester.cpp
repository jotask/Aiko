#include <catch2/catch_test_macros.hpp>

#include <lexer.h>
#include <parser.h>
#include <compiler_helper.h>

#include "test_types.h"
#include "tests_code.h"

TEST_CASE("Parser print string", "[PARSER]" )
{
    const char* code = test_code_print_string;

    INIT_LEXER

    printToken(token);
    while (token.kind != TokenKind::END)
    {
        token = lex.next();
        printToken(token);
        tokens.push_back(token);
    }
    auto parser = Parser(tokens);
    auto ast = parser.program();
    ast->print();
    REQUIRE_FALSE(true);
}

TEST_CASE("Parser if", "[PARSER]" )
{
    const char* code = test_code_basic_if;

    INIT_LEXER

    printToken(token);
    while (token.kind != TokenKind::END)
    {
        token = lex.next();
        printToken(token);
        tokens.push_back(token);
    }
    auto parser = Parser(tokens);
    auto ast = parser.program();
    ast->print();
    REQUIRE_FALSE(true);
}


TEST_CASE("Parser basic operations", "[PARSER]" )
{
    const char* code = test_code_basic_operations;

    INIT_LEXER

    printToken(token);
    while (token.kind != TokenKind::END)
    {
        token = lex.next();
        printToken(token);
        tokens.push_back(token);
    }
    auto parser = Parser(tokens);
    auto ast = parser.program();
    ast->print();
    REQUIRE_FALSE(true);
}

TEST_CASE("Parser basic LET and SET", "[PARSER]" )
{
    const char* code = test_code_basic_let_and_set;

    INIT_LEXER

    printToken(token);
    while (token.kind != TokenKind::END)
    {
        token = lex.next();
        printToken(token);
        tokens.push_back(token);
    }
    auto parser = Parser(tokens);
    auto ast = parser.program();
    ast->print();
    REQUIRE_FALSE(true);
}
