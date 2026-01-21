#pragma once

#include "compiler_types.h"

#include <logger/logger.h>
#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    inline void printToken(const Token token)
    {
        logger::Log::info("Token: %s -> %s", magic_enum::enum_name(token.kind).data(), token.text.c_str());
    }

}