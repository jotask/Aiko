#pragma once

#include "events/event.hpp"

namespace aiko
{

    class WindowCloseEvent : public Event
    {
    public:
        virtual ~WindowCloseEvent() = default;
    };

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(const int w, const int h)
            : width(w)
            , height(h)
        {

        };

        virtual ~WindowResizeEvent() = default;

        const int width;
        const int height;
    };

    class OnMouseEnterEvent : public Event
    {
    public:
        OnMouseEnterEvent(const bool enter) : entered(enter) {};
        virtual ~OnMouseEnterEvent() = default;
        const bool entered;
    };

    class OnKeyPressedEvent : public Event
    {
    public:
        OnKeyPressedEvent(int key, int scancode, int action, int mods) : key(key), scancode(scancode), action(action), mods(mods) {  };
        virtual ~OnKeyPressedEvent() = default;
        const int key;
        const int scancode;
        const int action;
        const int mods;
    };

    class OnMouseKeyPressedEvent : public Event
    {
    public:
        OnMouseKeyPressedEvent(int button, int action, int mods) : button(button), action(action), mods(mods) {  };
        virtual ~OnMouseKeyPressedEvent() = default;
        const int button;
        const int action;
        const int mods;
    };

    class OnMouseMoveEvent : public Event
    {
    public:
        OnMouseMoveEvent(float x, float y) : x(x), y(y) {};
        virtual ~OnMouseMoveEvent() = default;
        const float x;
        const float y;
    };

    class OnMouseScrollEvent : public Event
    {
    public:
        OnMouseScrollEvent(double xoffset, double yoffset) : xoffset(xoffset), yoffset(yoffset) {};
        virtual ~OnMouseScrollEvent() = default;
        const double xoffset;
        const double yoffset;
    };

}