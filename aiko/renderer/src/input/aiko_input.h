#pragma once

#include <map>

#include <math/math.h>

#include "inputs_types.h"
#include "display/display_events.hpp"

namespace aiko
{
    class AikoInput
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

    public:

        void init(GLFWwindow* window);
        void pollEvents();
        void clearEvents();

    private:

        struct InputType
        {
            PressedType Type;
            bool justPressed;
        };

        std::map<Key, InputType> m_keys_inputs;
        std::map<MouseButton, InputType> m_mouse_inputs;

        void onKeyPressed(OnKeyPressedEvent& event);
        void onMouseKeyPressed(OnMouseKeyPressedEvent& event);
        void onMouseMoved(OnMouseMoveEvent& event);
        void OnMouseScrollCallback(OnMouseScrollEvent& event);

        PressedType convertToAction(int code);

        GLFWwindow* m_window = nullptr;
        vec2 m_mousePosition;
        vec2 m_mouseScrollBack;
        vec2 m_mouseDelta;
        bool m_mouseCentred;
    };

}
