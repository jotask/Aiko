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

        bool isKeyPressed(Key key) const;
        bool isKeyJustPressed(Key key) const;
        bool isKeyJustReleased(Key key) const;

        vec2 getMouseWindowPosition() const;
        vec2 getMouseDelta() const;
        vec2 getMouseScrollDelta() const;

        bool isMouseButtonPressed(MouseButton button) const;
        bool isMouseButtonJustPressed(MouseButton button) const;
        bool isMouseButtonJustReleased(MouseButton button) const;

        void setMouseCaptured(bool captured);
        bool isMouseCaptured() const;

    public:

        void init(GLFWwindow* window);
        void pollEvents();
        void clearFrameState();

    private:

        std::map<Key, InputState> m_keys_inputs;
        std::map<MouseButton, InputState> m_mouse_inputs;

        void onKeyInput(OnKeyPressedEvent& event);
        void onMouseButtonInput(OnMouseKeyPressedEvent& event);
        void onMouseMoved(OnMouseMoveEvent& event);
        void onMouseScrolled(OnMouseScrollEvent& event);

        PressedType convertToAction(int code);

        GLFWwindow* m_window = nullptr;

        vec2 m_mouseWindowPosition  = {};
        vec2 m_mouseScrollDelta = {};
        vec2 m_mouseDelta = {};

        bool m_mouseCaptured = false;
    };

}
