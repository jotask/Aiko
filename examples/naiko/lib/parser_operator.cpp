#include "parser.h"

#include <logger/logger.h>

#include <magic_enum/magic_enum.hpp>

namespace aiko::naiko
{

    static void printStatement(NaikoOperation key, string text)
    {
        const auto name = magic_enum::enum_name(key);
        logger::Log::warning("STATEMENT-OPERATOR-%s: [%s]", name.data(), text.c_str());
    }

    void Parser::processOperator()
    {
        const auto current = getCurrentToken();
        if (const NaikoOperation* op = std::get_if<NaikoOperation>(&current.naiko))
        {
            printStatement(*op, current.text);
        }
        else
        {
            AIKO_ASSERT(false, "Found an unknow operator?");
        }
        next();
    }

}