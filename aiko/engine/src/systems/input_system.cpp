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

    void InputSystem::setIsMouseCentred(bool centred) const
    {
        m_inputModule->input().setCentredToScreen(centred);
    }

    bool InputSystem::getIsMouseCentred() const
    {
        return m_inputModule->input().getCentredToScreen();
    }

    bool InputSystem::isKeyPressed(Key key) const
    {
        return m_inputModule->input().isKeyPressed(key);
    }

    bool InputSystem::isKeyJustPressed(Key key) const
    {
        return m_inputModule->input().isKeyJustPressed(key);
    }

    vec2 InputSystem::getMousePosition() const
    {
        return m_inputModule->input().getMousePosition();
    }

    vec2 InputSystem::getMouseDelta() const
    {
        return m_inputModule->input().getMouseDelta();
    }

    vec2 InputSystem::getMouseScrollBack() const
    {
        return m_inputModule->input().getMouseScrollBack();
    }

    bool InputSystem::isMouseButtonPressed(MouseButton button) const
    {
        return m_inputModule->input().isMouseButtonPressed(button);
    }

}
