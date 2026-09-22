#include "input_system.h"

#include <input/aiko_input.h>

#include "modules/module_connector.h"
#include "modules/input_module.h"

namespace aiko
{

    void InputSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(InputModule, moduleConnector, m_inputModule)
    }

    void InputSystem::setMouseCaptured(bool captured) const
    {
        m_inputModule->input().setMouseCaptured(captured);
    }

    bool InputSystem::isMouseCaptured() const
    {
        return m_inputModule->input().isMouseCaptured();
    }

    bool InputSystem::isKeyPressed(Key key) const
    {
        return m_inputModule->input().isKeyPressed(key);
    }

    bool InputSystem::isKeyJustPressed(Key key) const
    {
        return m_inputModule->input().isKeyJustPressed(key);
    }

    bool InputSystem::isKeyJustReleased(Key key) const
    {
        return m_inputModule->input().isKeyJustReleased(key);
    }

    vec2 InputSystem::getMousePosition() const
    {
        return m_inputModule->input().getMousePosition();
    }

    vec2 InputSystem::getMouseDelta() const
    {
        return m_inputModule->input().getMouseDelta();
    }

    vec2 InputSystem::getMouseScrollDelta() const
    {
        return m_inputModule->input().getMouseScrollDelta();
    }

    bool InputSystem::isMouseButtonPressed(MouseButton button) const
    {
        return m_inputModule->input().isMouseButtonPressed(button);
    }

    bool InputSystem::isMouseButtonJustPressed(MouseButton button) const
    {
        return m_inputModule->input().isMouseButtonJustPressed(button);
    }

    bool InputSystem::isMouseButtonJustReleased(MouseButton button) const
    {
        return m_inputModule->input().isMouseButtonJustReleased(button);
    }

}
