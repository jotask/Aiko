#pragma once

#include <map>

#include <math/math.h>
#include <events/event.hpp>
#include <core/singleton.h>

#include "inputs_types.h"

namespace aiko
{
    class AikoInput : public Singleton<AikoInput>
    {
    public:

        AikoInput();
        ~AikoInput() = default;

        bool isKeyPressed(Key) const;
        bool isKeyJustPressed(Key) const;
        vec2 getMousePosition() const;
        vec2 getMouseDelta() const;
        vec2 getMouseScrollBack() const;
        bool isMouseButtonPressed(MouseButton button) const;

        void setCentredToScreen(bool);
        bool getCentredToScreen() const;

    protected:

        void init();
        void pollEvents();

    private:

        struct InputType
        {
            PressedType Type;
            bool justPressed;
        };

        std::map<Key, InputType> m_keys_inputs;
        std::map<MouseButton, InputType> m_mouse_inputs;

        void onKeyPressed(Event& event);
        void onMouseKeyPressed(Event& event);
        void onMouseMoved(Event& event);
        void OnMouseScrollCallback(Event& event);

        PressedType convertToAction(int code);

        vec2 m_mousePosition;
        vec2 m_mouseScrollBack;
        vec2 m_mouseDelta;
        bool m_mouseCentred;
    };

}
