#include "input_system.h"

#include "modules/module_connector.h"

#include "modules/render_module.h"
#include "modules/scene_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "types/camera_types.h"
#include "models/camera.h"
#include "modules/input_module.h"

namespace aiko
{

    void InputSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(InputModule, moduleConnector, m_inputModule)
    }
    
    void InputSystem::init()
    {
    
    }

    bool InputSystem::isKeyPressed(Key key) const
    {
        return m_inputModule->isKeyPressed(key);
    }

    vec2 InputSystem::getMousePosition() const
    {
        return vec2();
    }

    bool InputSystem::isMouseButtonPressed(MouseButton button) const
    {
        return false;
    }

}
