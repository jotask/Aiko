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
        OnKeyPressedEvent() { };
        OnKeyPressedEvent(int key, int scancode, int action, int mods) : key(key), scancode(scancode), action(action), mods(mods) {  };
        virtual ~OnKeyPressedEvent() = default;
        virtual EventId     getId() const { return "OnKeyPressedEvent"; }
        int key; int scancode; int action; int mods;
    };

    class OnMouseKeyPressedEvent : public Event
    {
    public:
        OnMouseKeyPressedEvent() { };
        OnMouseKeyPressedEvent(int button, int action, int mods) : button(button), action(action), mods(mods) {  };
        virtual ~OnMouseKeyPressedEvent() = default;
        virtual EventId     getId() const { return "OnMouseKeyPressedEvent"; }
        int button; int action; int mods;
    };

    class OnMouseMoveEvent : public Event
    {
    public:
        OnMouseMoveEvent() { };
        OnMouseMoveEvent(float x, float y) : x(x), y(y) {};
        virtual ~OnMouseMoveEvent() = default;
        virtual EventId     getId() const { return "OnMouseMoveEvent"; }
        float x; float y;
    };

    class OnMouseScrollCallbackEvent : public Event
    {
    public:
        OnMouseScrollCallbackEvent() { };
        OnMouseScrollCallbackEvent(double xoffset, double yoffset) : xoffset(xoffset), yoffset(yoffset) {};
        virtual ~OnMouseScrollCallbackEvent() = default;
        virtual EventId     getId() const { return "OnMouseScrollCallbackEvent"; }
        double xoffset; double yoffset;
    };

}