#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    void Parser::statement()
    {
        const auto current = getCurrentToken();
        switch (current.kind)
        {
        case TokenKind::END:
            {
                logger::Log::warning("END");
            } break;
        case TokenKind::KEYWORD:
            {
                processKeyword();
            } break;
        case TokenKind::VALUE:
            {
                processValue();
            } break;
        case TokenKind::OPERATOR:
            {
                processOperator();
            } break;
        default:
            AIKO_ASSERT(false, "unknow");
        }
    }

}