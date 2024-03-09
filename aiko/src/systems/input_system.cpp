#include "input_system.h"

#include "modules/module_connector.h"

#include "modules/render_module.h"
#include "modules/scene_module.h"
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

    bool InputSystem::isKeyPressed(Key key) const
    {
        return m_inputModule->isKeyPressed(key);
    }

    vec2 InputSystem::getMousePosition() const
    {
        return m_inputModule->getMousePosition();
    }

    bool InputSystem::isMouseButtonPressed(MouseButton button) const
    {
        return m_inputModule->isMouseButtonPressed(button);
    }

}
