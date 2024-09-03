#include "modules/input_module.h"

#include "models/time.h"

#include "aiko_types.h"
#include "core/libs.h"
#include "core/log.h"
#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "events/events.hpp"

#define LOG_INPUT false

namespace aiko
{

    bool InputModule::isKeyPressed(Key key) const
    {
        if (m_keys_inputs.find(key) != m_keys_inputs.end())
        {
            InputType::PressedType type = m_keys_inputs.at(key).Type;
            return type == InputType::PressedType::PRESS || type == InputType::PressedType::REPEAT;
        }
        return false;
    }

    bool InputModule::isKeyJustPressed(Key key) const
    {
        if (m_keys_inputs.find(key) != m_keys_inputs.end())
        {
            return m_keys_inputs.at(key).justPressed;
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

    vec2 InputModule::getMouseScrollBack() const
    {
        return m_mouseScrollBack;
    }

    bool InputModule::isMouseButtonPressed(MouseButton button) const
    {
        if (m_mouse_inputs.find(button) != m_mouse_inputs.end())
        {
            InputType::PressedType type = m_mouse_inputs.at(button).Type;
            return type == InputType::PressedType::PRESS || type == InputType::PressedType::REPEAT;
        }
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
        EventSystem::it().bind<OnMouseKeyPressedEvent>(this, &InputModule::onMouseKeyPressed);
        EventSystem::it().bind<OnMouseMoveEvent>(this, &InputModule::onMouseMoved);
        EventSystem::it().bind<OnMouseScrollCallbackEvent>(this, &InputModule::OnMouseScrollCallback);
        GLFWwindow* window = (GLFWwindow*)m_displayModule->getNativeDisplay();
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        setCentredToScreen(false);
    }

    void InputModule::endFrame()
    {
        GLFWwindow* window = (GLFWwindow*)m_displayModule->getNativeDisplay();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        for (auto it = m_keys_inputs.begin(); it != m_keys_inputs.end(); it++)
        {
            it->second.justPressed = false;
        }
        m_mouseDelta = {};
        m_mouseScrollBack = {};
    }

    void InputModule::onKeyPressed(Event& event)
    {
        const auto& msg = static_cast<const OnKeyPressedEvent&>(event);

        const aiko::Key key = static_cast<Key>(key);
        const InputType::PressedType action = convertToAction(msg.action);

        if(LOG_INPUT)
        {
            const char* keyo = magic_enum::enum_name<aiko::Key>(key).data();
            const char* actiono = magic_enum::enum_name<InputType::PressedType>(action).data();
            if (keyo != nullptr && actiono != nullptr)
            {
                Log::trace("KEY :: ACTION :: ", keyo, " :: ", actiono);
            }
            else
            {
                Log::trace("KEY :: ACTION :: UNKNOW KEY");
            }
        }

        m_keys_inputs[key].Type = action;
        m_keys_inputs[key].justPressed = action == InputType::PressedType::PRESS;

    }

    void InputModule::onMouseKeyPressed(Event& event)
    {
        const auto& msg = static_cast<const OnMouseKeyPressedEvent&>(event);

        const aiko::MouseButton key = static_cast<MouseButton>(msg.button);
        const InputType::PressedType action = convertToAction(msg.action);

        if (LOG_INPUT)
        {
            const char* keyo = magic_enum::enum_name<aiko::MouseButton>(key).data();
            const char* actiono = magic_enum::enum_name<InputType::PressedType>(action).data();
            if (keyo != nullptr && actiono != nullptr)
            {
                Log::trace("MOUSE :: BUTTON :: ", keyo, " :: ", actiono);
            }
            else
            {
                Log::trace("MOUSE :: BUTTON :: UNKNOW KEY");
            }
        }

        m_mouse_inputs[key].Type = action;
        m_mouse_inputs[key].justPressed = action == InputType::PressedType::PRESS;

    }

    void InputModule::onMouseMoved(Event& event)
    {
        const auto& msg = static_cast<const OnMouseMoveEvent&>(event);
        vec2 newMousePosition = { msg.x, msg.y };
        m_mouseDelta = newMousePosition - m_mousePosition;
        m_mousePosition = newMousePosition;
    }

    void InputModule::OnMouseScrollCallback(Event& event)
    {
        const auto& msg = static_cast<const OnMouseScrollCallbackEvent&>(event);
        m_mouseScrollBack = { static_cast<float>( msg.xoffset ), static_cast<float>( msg.yoffset ) };
    }

    InputModule::InputType::PressedType InputModule::convertToAction(int action)
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
    }

}
