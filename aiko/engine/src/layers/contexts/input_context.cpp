#include "input_context.h"

#include "systems/input_system.h"
#include "systems/system_connector.h"

namespace aiko
{
    InputContext::InputContext(SystemConnector& connector)
    {
        m_inputSystem = connector.find<InputSystem>();
        AIKO_ASSERT(
            m_inputSystem != nullptr,
            "Required system InputSystem not found"
        );
    }

    bool InputContext::isKeyPressed(Key key) const
    {
        return m_inputSystem->isKeyPressed(key);
    }

    bool InputContext::isKeyJustPressed(Key key) const
    {
        return m_inputSystem->isKeyJustPressed(key);
    }

    bool InputContext::isKeyJustReleased(Key key) const
    {
        return m_inputSystem->isKeyJustReleased(key);
    }

    vec2 InputContext::getMousePosition() const
    {
        return m_inputSystem->getMousePosition();
    }

    vec2 InputContext::getMouseDelta() const
    {
        return m_inputSystem->getMouseDelta();
    }

    vec2 InputContext::getMouseScrollDelta() const
    {
        return m_inputSystem->getMouseScrollDelta();
    }

    bool InputContext::isMouseButtonPressed(MouseButton button) const
    {
        return m_inputSystem->isMouseButtonPressed(button);
    }

    bool InputContext::isMouseButtonJustPressed(
        MouseButton button) const
    {
        return m_inputSystem->isMouseButtonJustPressed(button);
    }

    bool InputContext::isMouseButtonJustReleased(
        MouseButton button) const
    {
        return m_inputSystem->isMouseButtonJustReleased(button);
    }

    void InputContext::setMouseCaptured(bool captured) const
    {
        m_inputSystem->setMouseCaptured(captured);
    }

    bool InputContext::isMouseCaptured() const
    {
        return m_inputSystem->isMouseCaptured();
    }
}