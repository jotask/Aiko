#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    NodePtr Parser::processStatement()
    {
        const auto current = getCurrentToken();
        switch (current.kind)
        {
        case TokenKind::END:
            {
                return std::make_unique<EndNode>();
            }
        case TokenKind::KEYWORD:
            {
                return processKeyword();
            }
        case TokenKind::FUNCTION:
            {
                return processFunction();
            }
        default:
            AIKO_ASSERTF(false, "Unexpected token in statement: %s", current.text.c_str())
        }
        return nullptr;
    }

}