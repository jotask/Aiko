#pragma once

#include <typeindex>

namespace aiko
{
    class Event
    {
    public:
        using EventId = std::type_index;
        Event() = default;
        virtual ~Event() = default;
        bool handled = false;
    };

}
