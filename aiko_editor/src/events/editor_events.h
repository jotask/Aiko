#pragma once

#include <aiko_includes.h>

#include "events/event.hpp"

namespace aiko
{
    namespace editor
    {
        
        class HirearchyGameObjectSelectedEvent : public Event
        {
        public:
            HirearchyGameObjectSelectedEvent() = default;
            HirearchyGameObjectSelectedEvent(GameObject* obj) : selected(obj) {};
            virtual ~HirearchyGameObjectSelectedEvent() = default;
            virtual EventId     getId() const { return "HirearchyGameObjectSelectedEvent"; }
            GameObject* selected = nullptr;
       };    
    }
}