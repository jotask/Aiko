#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    static void printStatement(NaikoType key, string text)
    {
        const auto name = magic_enum::enum_name(key);
        logger::Log::warning("STATEMENT-VALUE-%s: [%s]", name.data(), text.c_str());
    }

    void Parser::processValue()
    {
        const auto current = getCurrentToken();
        if (const NaikoType* typ = std::get_if<NaikoType>(&current.naiko))
        {
            printStatement(*typ, current.text);
        }
        else
        {
            AIKO_ASSERT(false, "Found an unknow keyword?");
        }
        next();
    }

}