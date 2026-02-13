#pragma once

using namespace aiko::naiko;

struct Expected
{
    TokenKind kind;
    Naiko naiko;
    aiko::string text;
};

#define OP(naiko, text)         { TokenKind::OPERATOR,      naiko,  text },
#define KEYWORD(naiko, text)    { TokenKind::KEYWORD,       naiko,  text },
#define VALUE(naiko, text)      { TokenKind::VALUE,         naiko,  text },
#define SYMBOL(naiko, text)     { TokenKind::SYMBOL,        naiko,  text },
#define IDENTIFIER(text)        { TokenKind::IDENTIFIER,    {},     text },
#define END()                   { TokenKind::END,           {},     {}   },

#define INIT_LEXER_BASIC                                                \
        Lexer lex(code);                                                \
        Token token = lex.next();

#define INIT_LEXER                                                      \
        Tokenization tokens;                                            \
        Lexer lex(code);                                                \
        Token token = lex.next();                                       \
        tokens.push_back(token);

#define CHECK_TEST_TOKEN_LEXER(...) checkCurrentTokenIteration(__VA_ARGS__, false)
#define CHECK_TEST_TOKEN_PARSER(...) checkCurrentTokenIteration(__VA_ARGS__, true)

static void checkCurrentTokenIteration(std::vector<Expected> expecteds, std::vector<Token> tokens, size_t idx, bool testNaiko)
{
    const auto exp = expecteds[idx];
    const auto cur = tokens[idx];
    REQUIRE(exp.kind == cur.kind);

    if (testNaiko == false)
    {
        return;
    }

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
                AIKO_NOT_IMPLEMENTED;
            }
            break;
        default:
            break;
    }

}
