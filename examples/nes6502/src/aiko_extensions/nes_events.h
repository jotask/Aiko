#pragma once

#include "events/event.hpp"

namespace nes
{
    class NesOnClockEvent : public aiko::Event
    {
    public:
        NesOnClockEvent() = default;
        virtual ~NesOnClockEvent() = default;
        virtual EventId     getId() const { return "NesOnClockEvent"; }
    };
}