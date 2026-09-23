#pragma once

#include <input/inputs_types.h>
#include <math/math.h>

namespace aiko
{
    class InputSystem;
    class SystemConnector;

    class InputContext
    {
    public:
        bool isKeyPressed(Key key) const;
        bool isKeyJustPressed(Key key) const;
        bool isKeyJustReleased(Key key) const;

        vec2 getMouseWindowPosition() const;
        vec2 getMouseFramebufferPosition() const;
        vec2 getMouseDelta() const;
        vec2 getMouseScrollDelta() const;

        bool isMouseButtonPressed(MouseButton button) const;
        bool isMouseButtonJustPressed(MouseButton button) const;
        bool isMouseButtonJustReleased(MouseButton button) const;

        void setMouseCaptured(bool captured) const;
        bool isMouseCaptured() const;

    private:
        friend class LayerContext;

        explicit InputContext(SystemConnector& connector);

        InputSystem* m_inputSystem = nullptr;
    };
}
