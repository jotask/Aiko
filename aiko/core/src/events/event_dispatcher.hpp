#pragma once

#include "core/singleton.h"
#include "events/event.hpp"
#include "logger/logger.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace aiko
{

    class EventSystem : public Singleton<EventSystem>
    {
    public:

        EventSystem() = default;

        ~EventSystem() override
        {
            if (m_slots.empty())
            {
                return;
            }
            AIKO_LOG_WARNING_FMT("EventSystem destroyed with {} active binding(s)",m_slots.size());
            for (const auto& [id, slot] : m_slots)
            {
                AIKO_LOG_WARNING_FMT("Active event binding: id={} event={} owner={}", id, slot.eventId.name(), slot.object);
            }
        }

        template<class Evnt>
        void bind(void(*fun)(const Evnt&));

        template<class Evnt>
        void unbind(void(*fun)(const Evnt&));

        template<class Evnt, class T>
        void bind(T* object, void(T::*mf)(const Evnt&));

        template<class Evnt, class T>
        void unbind(T* object, void(T::*mf)(const Evnt&));

        void bindAny(void(*fun)(const Event&));

        void unbindAny(void(*fun)(const Event&));

        template<class T>
        void bindAny(T* object, void(T::*mf)(const Event&));

        template<class T>
        void unbindAny(T* object, void(T::*mf)(const Event&));

        void unbindAll(const void* object);

        template<class Evnt>
        void sendEvent(const Evnt& event);


    private:

        using SlotId = std::uint64_t;
        using Route = std::vector<SlotId>;


        enum class SlotKind
        {
            FreeFunction,
            MemberFunction
        };


        struct Slot
        {
            SlotId id = 0;
            Event::EventId eventId = Event::EventId(typeid(void));
            SlotKind kind = SlotKind::FreeFunction;
            bool any = false;
            const void* object = nullptr;
            std::type_index callbackType = std::type_index(typeid(void));
            std::function<bool(const void*)> matchesCallback;
            std::function<void(const Event&)> invoke;
        };


        std::unordered_map<SlotId, Slot> m_slots;
        std::unordered_map<Event::EventId, Route> m_routes;
        Route m_anyRoute;
        SlotId m_nextId = 1;


        void addSlot(Event::EventId eventId, SlotKind kind, bool any, const void* object, std::type_index callbackType, std::function<bool(const void*)> matchesCallback, std::function<void(const Event&)> invoke);
        void removeSlot(SlotId id);
        void dispatch(const Route& route, const Event& event);
    };


    inline void EventSystem::addSlot(Event::EventId eventId, SlotKind kind, bool any, const void* object, std::type_index callbackType, std::function<bool(const void*)> matchesCallback, std::function<void(const Event&)> invoke)
    {
        const SlotId id = m_nextId++;
        Slot slot
        {
            .id = id,
            .eventId = eventId,
            .kind = kind,
            .any = any,
            .object = object,
            .callbackType = callbackType,
            .matchesCallback = std::move(matchesCallback),
            .invoke = std::move(invoke)
        };
        m_slots.emplace(id, std::move(slot));
        if (any)
        {
            m_anyRoute.push_back(id);
        }
        else
        {
            m_routes[eventId].push_back(id);
        }
    }


    inline void EventSystem::removeSlot(SlotId id)
    {
        const auto slotIt = m_slots.find(id);
        if (slotIt == m_slots.end())
        {
            return;
        }
        const Event::EventId eventId = slotIt->second.eventId;
        const bool any = slotIt->second.any;
        if (any)
        {
            std::erase(m_anyRoute, id);
        }
        else
        {
            const auto routeIt = m_routes.find(eventId);
            if (routeIt != m_routes.end())
            {
                std::erase(routeIt->second, id);
                if (routeIt->second.empty())
                {
                    m_routes.erase(routeIt);
                }
            }
        }
        m_slots.erase(slotIt);
    }


    inline void EventSystem::dispatch(const Route& route, const Event& event)
    {
        for (const SlotId id : route)
        {
            const auto slotIt = m_slots.find(id);
            if (slotIt == m_slots.end())
            {
                continue;
            }
            std::function<void(const Event&)> invoke = slotIt->second.invoke;
            invoke(event);
        }
    }


    template<class Evnt>
    inline void EventSystem::bind(void(*fun)(const Evnt&))
    {
        static_assert(std::is_base_of_v<Event, Evnt>, "Event type must derive from Event");
        using Callback = void(*)(const Evnt&);
        addSlot(
            Event::EventId(typeid(Evnt)),
            SlotKind::FreeFunction,
            false,
            nullptr,
            std::type_index(typeid(Callback)),
            [fun](const void* candidate)
            {
                return *static_cast<const Callback*>(candidate) == fun;
            },
            [fun](const Event& event)
            {
                fun(static_cast<const Evnt&>(event));
            });
    }


    template<class Evnt>
    inline void EventSystem::unbind(void(*fun)(const Evnt&))
    {
        static_assert(std::is_base_of_v<Event, Evnt>, "Event type must derive from Event");
        using Callback = void(*)(const Evnt&);
        const Callback callback = fun;
        const Event::EventId eventId(typeid(Evnt));
        const auto routeIt = m_routes.find(eventId);
        if (routeIt == m_routes.end())
        {
            return;
        }
        const Route route = routeIt->second;
        std::vector<SlotId> matches;
        for (const SlotId id : route)
        {
            const auto slotIt = m_slots.find(id);
            if (slotIt == m_slots.end())
            {
                continue;
            }
            const Slot& slot = slotIt->second;
            if (slot.kind != SlotKind::FreeFunction || slot.callbackType != std::type_index(typeid(Callback)))
            {
                continue;
            }
            if (slot.matchesCallback(&callback))
            {
                matches.push_back(id);
            }
        }
        for (const SlotId id : matches)
        {
            removeSlot(id);
        }
    }


    template<class Evnt, class T>
    inline void EventSystem::bind(T* object, void(T::*mf)(const Evnt&))
    {
        static_assert(std::is_base_of_v<Event, Evnt>, "Event type must derive from Event");
        using Callback = void(T::*)(const Evnt&);
        addSlot(
            Event::EventId(typeid(Evnt)),
            SlotKind::MemberFunction,
            false,
            static_cast<const void*>(object),
            std::type_index(typeid(Callback)),
            [mf](const void* candidate)
            {
                return *static_cast<const Callback*>(candidate) == mf;
            },
            [object, mf](const Event& event)
            {
                (object->*mf)(static_cast<const Evnt&>(event));
            });
    }


    template<class Evnt, class T>
    inline void EventSystem::unbind(T* object, void(T::*mf)(const Evnt&))
    {
        static_assert(std::is_base_of_v<Event, Evnt>, "Event type must derive from Event");
        using Callback = void(T::*)(const Evnt&);
        const Callback callback = mf;
        const Event::EventId eventId(typeid(Evnt));
        const auto routeIt = m_routes.find(eventId);
        if (routeIt == m_routes.end())
        {
            return;
        }
        const Route route = routeIt->second;
        std::vector<SlotId> matches;
        for (const SlotId id : route)
        {
            const auto slotIt = m_slots.find(id);
            if (slotIt == m_slots.end())
            {
                continue;
            }
            const Slot& slot = slotIt->second;
            if (slot.kind != SlotKind::MemberFunction || slot.object != static_cast<const void*>(object) || slot.callbackType != std::type_index(typeid(Callback)))
            {
                continue;
            }
            if (slot.matchesCallback(&callback))
            {
                matches.push_back(id);
            }
        }
        for (const SlotId id : matches)
        {
            removeSlot(id);
        }
    }


    inline void EventSystem::bindAny(void(*fun)(const Event&))
    {
        using Callback = void(*)(const Event&);
        addSlot(
            Event::EventId(typeid(void)),
            SlotKind::FreeFunction,
            true,
            nullptr,
            std::type_index(typeid(Callback)),
            [fun](const void* candidate)
            {
                return *static_cast<const Callback*>(candidate) == fun;
            },
            [fun](const Event& event)
            {
                fun(event);
            });
    }

    inline void EventSystem::unbindAny(void(*fun)(const Event&))
    {
        using Callback = void(*)(const Event&);
        const Callback callback = fun;
        const Route route = m_anyRoute;
        std::vector<SlotId> matches;
        for (const SlotId id : route)
        {
            const auto slotIt = m_slots.find(id);
            if (slotIt == m_slots.end())
            {
                continue;
            }
            const Slot& slot = slotIt->second;
            if (slot.kind != SlotKind::FreeFunction || slot.callbackType != std::type_index(typeid(Callback)))
            {
                continue;
            }
            if (slot.matchesCallback( &callback))
            {
                matches.push_back(id);
            }
        }
        for (const SlotId id : matches)
        {
            removeSlot(id);
        }
    }


    template<class T>
    inline void EventSystem::bindAny(T* object, void(T::*mf)(const Event&))
    {
        using Callback = void(T::*)(const Event&);
        addSlot(
            Event::EventId(typeid(void)),
            SlotKind::MemberFunction,
            true,
            static_cast<const void*>(object),
            std::type_index(typeid(Callback)),
            [mf](const void* candidate)
            {
                return *static_cast<const Callback*>( candidate) == mf;
            },
            [object, mf](const Event& event)
            {
                (object->*mf)(event);
            });
    }


    template<class T>
    inline void EventSystem::unbindAny(T* object, void(T::*mf)(const Event&))
    {
        using Callback = void(T::*)(const Event&);
        const Callback callback = mf;
        const Route route = m_anyRoute;
        std::vector<SlotId> matches;
        for (const SlotId id : route)
        {
            const auto slotIt = m_slots.find(id);
            if (slotIt == m_slots.end())
            {
                continue;
            }
            const Slot& slot = slotIt->second;
            if (slot.kind != SlotKind::MemberFunction || slot.object != static_cast<const void*>(object) || slot.callbackType != std::type_index(typeid(Callback)))
            {
                continue;
            }
            if (slot.matchesCallback(&callback))
            {
                matches.push_back(id);
            }
        }
        for (const SlotId id : matches)
        {
            removeSlot(id);
        }
    }


    inline void EventSystem::unbindAll(const void* object)
    {
        if (object == nullptr)
        {
            return;
        }
        std::vector<SlotId> matches;
        for (const auto& [id, slot] : m_slots)
        {
            if (slot.kind == SlotKind::MemberFunction && slot.object == object)
            {
                matches.push_back(id);
            }
        }
        for (const SlotId id : matches)
        {
            removeSlot(id);
        }
    }


    template<class Evnt>
    inline void EventSystem::sendEvent(const Evnt& event)
    {
        static_assert(std::is_base_of_v<Event, Evnt>, "Event type must derive from Event");
        const Event::EventId eventId(typeid(Evnt));
        const Route anyRoute = m_anyRoute;
        Route typedRoute;
        if (const auto it = m_routes.find(eventId); it != m_routes.end())
        {
            typedRoute = it->second;
        }
        dispatch(anyRoute, event);
        dispatch(typedRoute, event);
    }

}
