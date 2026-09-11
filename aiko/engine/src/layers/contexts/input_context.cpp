#include "input_context.h"

#include "systems/input_system.h"
#include "systems/system_connector.h"

namespace aiko
{
    InputContext::InputContext(SystemConnector& connector)
    {
        m_inputSystem = connector.find<InputSystem>();
        AIKO_ASSERT(m_inputSystem != nullptr, "Required system InputSystem not found");
    }

    bool InputContext::isKeyPressed(Key key) const
    {
        return m_inputSystem->isKeyPressed(key);
    }

    bool InputContext::isKeyJustPressed(Key key) const
    {
        return m_inputSystem->isKeyJustPressed(key);
    }

    vec2 InputContext::getMousePosition() const
    {
        return m_inputSystem->getMousePosition();
    }

    vec2 InputContext::getMouseDelta() const
    {
        return m_inputSystem->getMouseDelta();
    }

    vec2 InputContext::getMouseScrollBack() const
    {
        return m_inputSystem->getMouseScrollBack();
    }

    bool InputContext::isMouseButtonPressed(MouseButton button) const
    {
        return m_inputSystem->isMouseButtonPressed(button);
    }

    void InputContext::setIsMouseCentred(bool centred) const
    {
        m_inputSystem->setIsMouseCentred(centred);
    }

    bool InputContext::getIsMouseCentred() const
    {
        return m_inputSystem->getIsMouseCentred();
    }
}
