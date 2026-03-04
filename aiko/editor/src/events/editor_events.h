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
            HirearchyGameObjectSelectedEvent(GameObject* obj) : selected(obj) {};
            virtual ~HirearchyGameObjectSelectedEvent() = default;
            const GameObject* selected = nullptr;
       };    
    }
}