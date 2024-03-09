#include "input_system.h"

#include "modules/module_connector.h"

#include "modules/render_module.h"
#include "modules/scene_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "types/camera_types.h"
#include "models/camera.h"
#include "modules/input_module.h"
#include "models/input.h"
#include "types/inputs.h"

namespace aiko
{

    void InputSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(InputModule, moduleConnector, m_inputModule)
    }
    
    void InputSystem::init()
    {
        typedef EnumIterator<aiko::Key, aiko::Key::KEY_NULL, aiko::Key::KEY_VOLUME_DOWN> keyIterator;
        auto& in = Input::it();
        for (aiko::Key i : keyIterator())
        {
            bool pressed = m_inputModule->isKeyPressed(i);
            in.pressedKeys.emplace(i, pressed);
        }
    }

    void InputSystem::update()
    {
        typedef EnumIterator<aiko::Key, aiko::Key::KEY_NULL, aiko::Key::KEY_VOLUME_DOWN> keyIterator;
        auto& in = Input::it();
        for (aiko::Key i : keyIterator())
        {
            bool pressed = m_inputModule->isKeyPressed(i);
            in.pressedKeys[i] = pressed;
        }
        in.mousePosition = m_inputModule->getMousePosition();
    }

    bool InputSystem::isKeyPressed(Key key) const
    {
        return m_inputModule->isKeyPressed(key);
    }

    vec2 InputSystem::getMousePosition() const
    {
        return Input::it().getMousePosition();
    }

    bool InputSystem::isMouseButtonPressed(MouseButton button) const
    {
        return m_inputModule->isMouseButtonPressed(button);
    }

}
