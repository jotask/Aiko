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
    parser.program();

    AIKO_TODO("Not Implemented");

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
    parser.program();

    AIKO_TODO("Not Implemented");

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
    parser.program();

    AIKO_TODO("Not Implemented");

}
