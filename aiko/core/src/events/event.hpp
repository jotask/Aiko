#pragma once

namespace aiko
{
    class Event
    {
    public:
        using EventId = const char*;
        Event() = default;
        virtual ~Event() = default;
        virtual EventId     getId() const = 0;
    };

}