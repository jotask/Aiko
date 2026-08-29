#pragma once

#include <aiko_types.h>

#include <limits>

namespace aiko
{
    struct EntityId
    {
        u32 value = std::numeric_limits<u32>::max();

        bool valid() const
        {
            return value != std::numeric_limits<u32>::max();
        }
    };
}
