#include "modules/input_module.h"

#include "models/time.h"

#include "aiko_types.h"
#include "core/libs.h"
#include "core/log.h"
#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "events/events.hpp"

namespace aiko
{

    bool InputModule::isKeyPressed(Key key) const
    {
        if (m_inputs.find(key) != m_inputs.end())
        {
            InputType::PressedType type = m_inputs.at(key).Type;
            return type == InputType::PressedType::PRESS || type == InputType::PressedType::REPEAT;
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
        return m_mousePosition;
    }

    vec2 InputModule::getMouseDelta() const
    {
        return m_mouseDelta;
    }

    bool InputModule::isMouseButtonPressed(MouseButton button) const
    {
        return false;
    }

    void InputModule::setCentredToScreen(bool newMouseCentred)
    {
        m_mouseCentred = newMouseCentred;
        GLFWwindow* window = (GLFWwindow*)m_displayModule->getNativeDisplay();
        if (m_mouseCentred == true)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    bool InputModule::getCentredToScreen() const
    {
        return m_mouseCentred;
    }

    void InputModule::connect(ModuleConnector* moduleConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule)
    }

    void InputModule::init()
    {
        EventSystem::it().bind<OnKeyPressedEvent>(this, &InputModule::onKeyPressed);
        EventSystem::it().bind<OnMouseMoveEvent>(this, &InputModule::onMouseMoved);
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
        m_mouseDelta = {};
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
                    Log::error("KEY :: ACTION :: Not Implemented");
                }
            };

        static auto toKey = [](int key) -> Key
            {
                return (Key)key;
            };

        const auto key = toKey(msg.key);
        const auto action = toAction(msg.action);

        {
            auto keyo = magic_enum::enum_name(key).data();
            auto actiono = magic_enum::enum_name(action).data();
            Log::trace("KEY :: ACTION :: ", keyo, " :: ", actiono);
        }

        m_inputs[key].Type = action;
        m_inputs[key].justPressed = action == InputType::PressedType::PRESS;

    }

    void InputModule::onMouseMoved(Event& event)
    {
        const auto& msg = static_cast<const OnMouseMoveEvent&>(event);
        vec2 newMousePosition = { msg.x, msg.y };
        m_mouseDelta = newMousePosition - m_mousePosition;
        m_mousePosition = newMousePosition;
    }

}
