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
