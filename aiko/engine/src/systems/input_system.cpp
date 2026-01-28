#include "input_system.h"

#include <input/aiko_input.h>

#include "modules/module_connector.h"
#include "modules/render_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "models/camera.h"
#include "modules/input_module.h"

namespace aiko
{

    void InputSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(InputModule, moduleConnector, m_inputModule)
    }

    void InputSystem::setIsMouseCentred(bool centred) const
    {
        AikoInput::it().setCentredToScreen(centred);
    }

    bool InputSystem::getIsMouseCentred() const
    {
        return AikoInput::it().getCentredToScreen();
    }

    bool InputSystem::isKeyPressed(Key key) const
    {
        return AikoInput::it().isKeyPressed(key);
    }

    bool InputSystem::isKeyJustPressed(Key key) const
    {
        return AikoInput::it().isKeyJustPressed(key);
    }

    vec2 InputSystem::getMousePosition() const
    {
        return AikoInput::it().getMousePosition();
    }

    vec2 InputSystem::getMouseDelta() const
    {
        return AikoInput::it().getMouseDelta();
    }

    vec2 InputSystem::getMouseScrollBack() const
    {
        return AikoInput::it().getMouseScrollBack();
    }

    bool InputSystem::isMouseButtonPressed(MouseButton button) const
    {
        return AikoInput::it().isMouseButtonPressed(button);
    }

}
