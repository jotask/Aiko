#pragma once

#include "core/singleton.h"
#include "events/event.hpp"

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <cstring>
#include <algorithm>

namespace aiko
{

    class EventSystem : public Singleton<EventSystem>
    {

    public:

        EventSystem() = default;
        ~EventSystem() override = default;

        template<class Evnt>
        void bind(void(*fun)(Evnt&));

        template<class Evnt>
        void unbind(void(*fun)(Evnt&));

        template<class Evnt, class T>
        void bind(T* object, void(T::*mf)(Evnt&));

        template<class Evnt, class T>
        void unbind(T* object, void(T::*mf)(Evnt&));

        // Bind all
        void bindAny(void(*fun)(Event&));
        void unbindAny(void(*fun)(Event&));

        template<class T>
        void bindAny(T* obj, void(T::*mf)(Event&));

        template<class T>
        void unbindAny(T* obj, void(T::*mf)(Event&));

        template<class Evnt>
        void sendEvent(Evnt& evnt);

    private:

        enum class SlotKind
        {
            FreeFn,
            MemberFn,
        };

        struct Slot
        {
            SlotKind kind = SlotKind::FreeFn;

            // Identity
            void* object = nullptr;
            void* freeFn = nullptr;

            static constexpr size_t MEMBER_STORAGE = 32;
            unsigned char memberBytes[MEMBER_STORAGE] {};
            size_t memberSize = 0;

            // Invocation
            void (*invoke)(Slot&, Event&) = nullptr;

        };

        std::unordered_map<Event::EventId, std::vector<Slot>> m_map;
        std::vector<Slot> m_any;

        template<class E>
        static Slot makeFreeSlot(void(*fnt)(E&));

        template<class T, class E>
        static Slot makeMemberSlot(T* obj, void(T::*mf)(E&));

        static bool memberEquals(const Slot& s, void* obj, const void* mfBytes, size_t mfSize);

    };

    inline bool EventSystem::memberEquals(const Slot& s, void* obj, const void* mfBytes, size_t mfSize)
    {
        return s.kind == SlotKind::MemberFn
            && s.object == obj
            && s.memberSize == mfSize
            && std::memcmp(s.memberBytes, mfBytes, mfSize) == 0;
    }

    template<class E>
    inline EventSystem::Slot EventSystem::makeFreeSlot(void(*fun)(E&))
    {
        Slot s
        {
            .kind = SlotKind::FreeFn,
            .freeFn = reinterpret_cast<void*>(fun),
            .invoke = [](Slot& slot, Event& e)
            {
                auto f = reinterpret_cast<void(*)(E&)>(slot.freeFn);
                f(static_cast<E&>(e));
            },
        };
        return s;
    }

    template<class T, class E>
    inline EventSystem::Slot EventSystem::makeMemberSlot(T* obj, void(T::*mf)(E&))
    {
        Slot s
        {
            .kind = SlotKind::MemberFn,
            .object = obj,
            .memberSize = sizeof(mf),
            .invoke = [](Slot& slot, Event& e)
            {
                void(T::*fn)(E&);
                std::memcpy(&fn, slot.memberBytes, sizeof(fn));
                (static_cast<T*>(slot.object)->*fn)(static_cast<E&>(e));
            },
        };
        static_assert(sizeof(mf) <= Slot::MEMBER_STORAGE, "Member function pointer too large for Slot::memberBytes. Increase storage.");
        std::memcpy(s.memberBytes, &mf, sizeof(mf));
        return s;
    }

    template<class Evnt>
    inline void EventSystem::bind(void(* const fun)(Evnt&))
    {
        m_map[Event::EventId(typeid(Evnt))].push_back(makeFreeSlot<Evnt>(fun));
    }

    template<class Evnt>
    inline void EventSystem::unbind(void(* const fun)(Evnt&))
    {
        auto it = m_map.find(Event::EventId(typeid(Evnt)));
        if (it == m_map.end()) return;

        void* id = reinterpret_cast<void*>(fun);
        auto& vec = it->second;

        vec.erase(std::remove_if(vec.begin(), vec.end(),
            [&](const Slot& s) { return s.kind == SlotKind::FreeFn && s.freeFn == id; }),
            vec.end());

        if (vec.empty()) m_map.erase(it);
    }

    template<class Evnt, class T>
    inline void EventSystem::bind(T* const object, void(T::* const mf)(Evnt&))
    {
        m_map[Event::EventId(typeid(Evnt))].push_back(makeMemberSlot<T, Evnt>(object, mf));
    }

    template<class Evnt, class T>
    inline void EventSystem::unbind(T* const object, void(T::* const mf)(Evnt&))
    {
        auto it = m_map.find(Event::EventId(typeid(Evnt)));
        if (it == m_map.end()) return;

        unsigned char bytes[sizeof(mf)];
        std::memcpy(bytes, &mf, sizeof(mf));

        auto& vec = it->second;
        vec.erase(std::remove_if(vec.begin(), vec.end(),
            [&](const Slot& s) { return memberEquals(s, object, bytes, sizeof(mf)); }),
            vec.end());

        if (vec.empty()) m_map.erase(it);
    }

    inline void EventSystem::bindAny(void(*fun)(Event&))
    {
        const Slot s
        {
            .kind = SlotKind::FreeFn,
            .freeFn = reinterpret_cast<void*>(fun),
            .invoke = [](Slot& slot, Event& e)
            {
                auto f = reinterpret_cast<void(*)(Event&)>(slot.freeFn);
                f(e);
            },
        };
        m_any.push_back(s);
    }

    template<class T>
    inline void EventSystem::bindAny(T* obj, void(T::*mf)(Event&))
    {
        Slot s
        {
            .kind = SlotKind::MemberFn,
            .object = obj,
            .memberSize = sizeof(mf),
            .invoke = [](Slot& slot, Event& e)
            {
                void(T::*fn)(Event&);
                std::memcpy(&fn, slot.memberBytes, sizeof(fn));
                (static_cast<T*>(slot.object)->*fn)(e);
            },
        };
        static_assert(sizeof(mf) <= sizeof(s.memberBytes), "Member function pointer too large for Slot::memberBytes. Increase storage.");
        std::memcpy(s.memberBytes, &mf, sizeof(mf));
        m_any.push_back(s);
    }

    template<class T>
    inline void EventSystem::unbindAny(T* obj, void(T::*mf)(Event&))
    {
        unsigned char bytes[sizeof(mf)];
        std::memcpy(bytes, &mf, sizeof(mf));

        m_any.erase(std::remove_if(m_any.begin(), m_any.end(),
            [&](const Slot& s) { return memberEquals(s, obj, bytes, sizeof(mf)); }),
            m_any.end());
    }

    inline void EventSystem::unbindAny(void(*fun)(Event&))
    {
        void* id = reinterpret_cast<void*>(fun);
        m_any.erase(std::remove_if(m_any.begin(), m_any.end(),
            [&](const Slot& s) { return s.kind == SlotKind::FreeFn && s.freeFn == id; }),
            m_any.end());
    }

    template<class Evnt>
    inline void EventSystem::sendEvent(Evnt& evnt)
    {
        for (auto& s : m_any)
        {
            s.invoke(s, evnt);
            if (evnt.handled)
            {
                return;
            }
        }

        auto it = m_map.find(Event::EventId(typeid(Evnt)));
        if (it == m_map.end())
        {
            return;
        }

        for (auto& s : it->second)
        {
            s.invoke(s, evnt);
            if (evnt.handled)
            {
                return;
            }
        }
    }

}