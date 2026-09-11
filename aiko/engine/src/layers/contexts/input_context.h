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

        vec2 getMousePosition() const;
        vec2 getMouseDelta() const;
        vec2 getMouseScrollBack() const;

        bool isMouseButtonPressed(MouseButton button) const;

        void setIsMouseCentred(bool centred) const;
        bool getIsMouseCentred() const;

    private:
        friend class LayerContext;

        explicit InputContext(SystemConnector& connector);

        InputSystem* m_inputSystem = nullptr;
    };
}
