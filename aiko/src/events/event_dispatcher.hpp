#pragma once

#include "shared/singleton.h"
#include "events/event.hpp"

#include "core/log.h"

#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <iostream>
#include <algorithm>

namespace aiko
{

    class EventSystem : public Singleton<EventSystem>
    {
    public:

        EventSystem() = default;
        ~EventSystem() = default;

        template<class Evnt>
        void bind(void(* const fun)(Event&));

        template<class Evnt>
        void unbind(void(* const fun)(Event&));

        template<class Evnt, class T>
        void bind(T* const object, void(T::* const mf)(Event&));

        template<class Evnt, class T>
        void unbind(T* const object, void(T::* const mf)(Event&));

        template<class Evnt>
        void sendEvent(Evnt& evnt);

    private:

        using CallbackFntParameters = void(Event&);
        using CallbackFnt = std::function<CallbackFntParameters>;
        using Callbacks = std::vector<CallbackFnt>;
        using CallbacksMap = std::map<const char*, Callbacks>;
        CallbacksMap m_map;

    };

    template<class Evnt>
    inline void EventSystem::bind(void(* const fun)(Event&))
    {
        const Evnt evnt;
        auto found = m_map.find(evnt.getId());
        if (found != m_map.end())
        {
            found->second.emplace_back(fun);
        }
        else
        {
            auto pair = std::make_pair<const char*, Callbacks>(evnt.getId(), { fun });
            m_map.insert(pair);
        }
    }

    template<class Evnt>
    inline void EventSystem::unbind(void(* const fun)(Event&))
    {
        const Evnt evnt;
        auto found = m_map.find(evnt.getId());
        if (found != m_map.end())
        {
            auto& callbacks = found->second;
            callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), fun), callbacks.end());

            // If the list of callbacks is empty, you can optionally remove the event from the map
            if (callbacks.empty())
            {
                m_map.erase(found);
            }
        }
    }

    template<class Evnt, class T>
    inline void EventSystem::bind(T* const object, void(T::* const mf)(Event&))
    {
        const Evnt evnt;
        auto found = m_map.find(evnt.getId());
        if (found != m_map.end())
        {
            found->second.emplace_back(std::bind(mf, object, std::placeholders::_1));
        }
        else
        {
            auto pair = std::make_pair<const char*, Callbacks>(evnt.getId(), { std::bind(mf, object, std::placeholders::_1) });
            m_map.insert(pair);
        }
    }

    template<class Evnt, class T>
    inline void EventSystem::unbind(T* const object, void(T::* const mf)(Event&))
    {
        const Evnt evnt;
        auto found = m_map.find(evnt.getId());
        if (found == m_map.end())
        {
            return;
        }

        using BoundType = decltype(std::bind(mf, object, std::placeholders::_1));

        auto& callbacks = found->second;
        callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(), [&](const CallbackFnt& f) {
            // Check if the target matches the bound type
            return f.target<BoundType>() != nullptr;
        }), callbacks.end());

        if (callbacks.empty())
        {
            m_map.erase(found);
        }
    }

    template<class Evnt>
    inline void EventSystem::sendEvent(Evnt& evnt)
    {
        auto found = m_map.find(evnt.getId());
        if (found != m_map.end())
        {
            auto& collection = found->second;
            for (auto& callback : collection)
            {
                callback(evnt);
            }
        }
    }

}