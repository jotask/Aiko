#pragma once

#include <aiko_includes.h>

#include "events/event.hpp"

namespace editor
{

    class HirearchyGameObjectSelectedEvent : public aiko::Event
    {
    public:
        HirearchyGameObjectSelectedEvent() = default;
        HirearchyGameObjectSelectedEvent(aiko::GameObject* obj) : selected(obj) {};
        virtual ~HirearchyGameObjectSelectedEvent() = default;
        aiko::GameObject* selected = nullptr;
        virtual EventId     getId() const { return "HirearchyGameObjectSelectedEvent"; }
    };

}
