#pragma once

#include "events/event.hpp"

namespace aiko
{

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;
        virtual ~WindowCloseEvent() = default;
        virtual EventId     getId() const { return "WindowCloseEvent"; }
    };

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent()
            : width(-1)
            , height(-1)
        {

        };

        WindowResizeEvent(const int w, const int h)
            : width(w)
            , height(h)
        {

        };

        virtual ~WindowResizeEvent() = default;
        virtual EventId     getId() const { return "WindowResizeEvent"; }

        const int width;
        const int height;
    };

    class OnMouseEnterEvent : public Event
    {
    public:
        OnMouseEnterEvent() : entered(false) {};
        OnMouseEnterEvent(const bool enter) : entered(enter) {};
        virtual ~OnMouseEnterEvent() = default;
        virtual EventId     getId() const { return "OnMouseEnterEvent"; }
        const bool entered;
    };

    class OnKeyPressedEvent : public Event
    {
    public:
        OnKeyPressedEvent() = default;
        OnKeyPressedEvent(int key, int scancode, int action, int mods) : key(key), scancode(scancode), action(action), mods(mods) {  };
        virtual ~OnKeyPressedEvent() = default;
        virtual EventId     getId() const { return "OnKeyPressedEvent"; }
        int key; int scancode; int action; int mods;
    };

    class OnMouseMoveEvent : public Event
    {
    public:
        OnMouseMoveEvent() = default;
        OnMouseMoveEvent(float x, float y, float prevX, float prevY) : x(x), y(y), prevX(prevX), prevY(prevY)
        {
            dx = x - prevX;
            dy = prevY - y;
        };
        virtual ~OnMouseMoveEvent() = default;
        virtual EventId     getId() const { return "OnMouseMoveEvent"; }
        float x; float y;
        float prevX; float prevY;
        float dx, dy;
    };

}