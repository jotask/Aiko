#include "input_system.h"

#include "modules/module_connector.h"

#include "modules/render_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "models/camera.h"
#include "modules/input_module.h"
#include "types/inputs.h"

namespace aiko
{

    void InputSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(InputModule, moduleConnector, m_inputModule)
    }

    void InputSystem::setIsMouseCentred(bool centred) const
    {
        m_inputModule->setCentredToScreen(centred);
    }

    bool InputSystem::getIsMouseCentred() const
    {
        return m_inputModule->getCentredToScreen();
    }

    bool InputSystem::isKeyPressed(Key key) const
    {
        return m_inputModule->isKeyPressed(key);
    }

    bool InputSystem::isKeyJustPressed(Key key) const
    {
        return m_inputModule->isKeyJustPressed(key);
    }

    vec2 InputSystem::getMousePosition() const
    {
        return m_inputModule->getMousePosition();
    }

    vec2 InputSystem::getMouseDelta() const
    {
        return m_inputModule->getMouseDelta();
    }

    bool InputSystem::isMouseButtonPressed(MouseButton button) const
    {
        return m_inputModule->isMouseButtonPressed(button);
    }

}
