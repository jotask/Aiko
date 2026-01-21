#include <catch2/catch_test_macros.hpp>

#include <lexer.h>
#include <parser.h>
#include <compiler_helper.h>

#include "test_types.h"
#include "tests_code.h"

TEST_CASE("Parser print string", "[PARSER]" )
{
    const char* code = test_code_print_string;

    std::vector<Expected> expecteds
    {
        KEYWORD(NaikoKeyword::PRINT, "PRINT" )
        VALUE(NaikoType::STRING, "\"hello, world!\"" )
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
    auto parser = Parser(tokens);
    parser.program();

    AIKO_TODO("Not Implemented");

}
