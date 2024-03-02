#pragma once

#include "shared/singleton.h"
#include "events/event.hpp"

#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <iostream>

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
        // TODO
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
        using BoundType = decltype(std::bind(mf, object, std::placeholders::_1));
        const auto found = [](const CallbackFnt& f) {
            const auto result = f.target<BoundType>() != nullptr;
            return result;
            };
        auto it = std::remove_if(m_callbacks.begin(), m_callbacks.end(), found);
        if (it == m_callbacks.end())
        {
            std::cout << "unbind error" << std::endl;
            return;
        }
        m_callbacks.erase(it);
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