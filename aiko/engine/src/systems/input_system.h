#pragma once

#include "systems/base_system.h"
#include "input/inputs_types.h"

namespace aiko
{

    class InputModule;

    class InputSystem : public BaseSystem
    {
    public:

        InputSystem() = default;
        virtual ~InputSystem() = default;

        void setMouseCaptured(bool captured) const;
        bool isMouseCaptured() const;

        bool isKeyPressed(Key key) const;
        bool isKeyJustPressed(Key key) const;
        bool isKeyJustReleased(Key key) const;

        vec2 getMousePosition() const;
        vec2 getMouseDelta() const;
        vec2 getMouseScrollDelta() const;

        bool isMouseButtonPressed(MouseButton button) const;
        bool isMouseButtonJustPressed(MouseButton button) const;
        bool isMouseButtonJustReleased(MouseButton button) const;

    protected:
        virtual SystemUpdatePhase updatePhase() const override { return SystemUpdatePhase::Input; }
        virtual void connect(ModuleConnector*, SystemConnector*) override;

    private:

        InputModule* m_inputModule = nullptr;

    };

}
