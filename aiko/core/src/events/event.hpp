#pragma once

#include <aiko_types.h>

namespace aiko
{
    class AIKO_API Event
    {
    public:
        using EventId = const char*;
        Event() = default;
        virtual ~Event() = default;
        virtual EventId     getId() const = 0;
    };

}