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
        if (m_inputs.find(key) != m_inputs.end())
        {
            return m_inputs.at(key).Type == InputType::PressedType::PRESS;
        }
        return false;
    }

    bool InputModule::isKeyJustPressed(Key key) const
    {
        if (m_inputs.find(key) != m_inputs.end())
        {
            return m_inputs.at(key).justPressed;
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

    void InputModule::endFrame()
    {
        GLFWwindow* window = (GLFWwindow*)m_displayModule->getNativeDisplay();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        for (auto it = m_inputs.begin(); it != m_inputs.end(); it++)
        {
            it->second.justPressed = false;
        }
    }

    void InputModule::onKeyPressed(Event& event)
    {
        const auto& msg = static_cast<const OnKeyPressedEvent&>(event);

        bool final = false;

        static auto toAction = [](int action) -> InputType::PressedType
            {

                if (action == GLFW_RELEASE)
                {
                    return InputType::PressedType::RELEASE;
                }
                else if (action == GLFW_PRESS)
                {
                    return InputType::PressedType::PRESS;
                }
                else if (action == GLFW_REPEAT)
                {
                    return InputType::PressedType::REPEAT;
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

        const auto key = toKey(msg.key);
        const auto action = toAction(msg.action);

        m_inputs[key].Type = action;
        m_inputs[key].justPressed = action == InputType::PressedType::PRESS;

        if (m_inputs[key].justPressed)
        {
            int a = 0;
        }

    }

}
