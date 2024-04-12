#include "modules/input_module.h"

#include "models/time.h"

#include "aiko_types.h"
#include "core/libs.h"
#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "events/events.hpp"

#include <GLFW/glfw3.h>

namespace aiko
{

    bool InputModule::isKeyPressed(Key key) const
    {
        if ( m_inputs.find( key ) != m_inputs.end() )
        {
            return m_inputs.at(key) == PressedType::PRESS;
        }
        return false;
    }

    vec2 InputModule::getMousePosition() const
    {
        return {};
    }

    bool InputModule::isMouseButtonPressed(MouseButton button) const
    {
        return false;
    }

    void InputModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule)
    }

    void InputModule::init()
    {
        EventSystem::it().bind<OnKeyPressedEvent>(this, &InputModule::onKeyPressed);
    }

    void InputModule::preUpdate()
    {
        GLFWwindow* window = (GLFWwindow*)m_displayModule->getNativeDisplay();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }

    void InputModule::onKeyPressed(Event& event)
    {
        const auto& msg = static_cast<const OnKeyPressedEvent&>(event);

        bool final = false;

        static auto toAction = [](int action)->PressedType
            {

                if (action == GLFW_RELEASE)
                {
                    return PressedType::RELEASE;
                }
                else if (action == GLFW_PRESS)
                {
                    return PressedType::PRESS;
                }
                else if (action == GLFW_REPEAT)
                {
                    return PressedType::REPEAT;
                }
                else
                {
                    std::cout << "KEY :: ACTION :: Not Implemented" << std::endl;
                }
            };

        static auto toKey = [](int key) -> Key
            {
                return (Key)key;
            };

        m_inputs[toKey(msg.key)] = toAction(msg.action);

        int a = 0;

    }

}
