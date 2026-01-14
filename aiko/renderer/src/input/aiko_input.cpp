#include "aiko_input.h"

#include <aiko_types.h>
#include <time/time.h>
#include <logger/logger.h>
#include <events/events.hpp>

#include "display/display_events.hpp"
#include "display/display_manager.h"

#define LOG_INPUT false

namespace aiko
{

    AikoInput::AikoInput()
    {

    }

    bool AikoInput::isKeyPressed(Key key) const
    {
        if (m_keys_inputs.find(key) != m_keys_inputs.end())
        {
            PressedType type = m_keys_inputs.at(key).Type;
            return type == PressedType::PRESS || type == PressedType::REPEAT;
        }
        return false;
    }

    bool AikoInput::isKeyJustPressed(Key key) const
    {
        if (m_keys_inputs.find(key) != m_keys_inputs.end())
        {
            return m_keys_inputs.at(key).justPressed;
        }
        return false;
    }

    vec2 AikoInput::getMousePosition() const
    {
        return m_mousePosition;
    }

    vec2 AikoInput::getMouseDelta() const
    {
        return m_mouseDelta;
    }

    vec2 AikoInput::getMouseScrollBack() const
    {
        return m_mouseScrollBack;
    }

    bool AikoInput::isMouseButtonPressed(MouseButton button) const
    {
        if (m_mouse_inputs.find(button) != m_mouse_inputs.end())
        {
            PressedType type = m_mouse_inputs.at(button).Type;
            return type == PressedType::PRESS || type == PressedType::REPEAT;
        }
        return false;
    }

    void AikoInput::setCentredToScreen(bool newMouseCentred)
    {
        m_mouseCentred = newMouseCentred;
        GLFWwindow* window = DisplayManager::it().getNativeWindow();
        if (m_mouseCentred == true)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    bool AikoInput::getCentredToScreen() const
    {
        return m_mouseCentred;
    }

    void AikoInput::init()
    {
        EventSystem::it().bind<OnKeyPressedEvent>(this, &AikoInput::onKeyPressed);
        EventSystem::it().bind<OnMouseKeyPressedEvent>(this, &AikoInput::onMouseKeyPressed);
        EventSystem::it().bind<OnMouseMoveEvent>(this, &AikoInput::onMouseMoved);
        EventSystem::it().bind<OnMouseScrollCallbackEvent>(this, &AikoInput::OnMouseScrollCallback);
        GLFWwindow* window = DisplayManager::it().getNativeWindow();
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        setCentredToScreen(false);
    }

    void AikoInput::pollEvents()
    {
        glfwPollEvents();
        GLFWwindow* window = DisplayManager::it().getNativeWindow();
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

    void AikoInput::onKeyPressed(Event& event)
    {
        const auto& msg = static_cast<const OnKeyPressedEvent&>(event);

        const Key key = static_cast<Key>(msg.key);
        const PressedType action = convertToAction(msg.action);

        if(LOG_INPUT)
        {
            const char* keyo = magic_enum::enum_name<Key>(key).data();
            const char* actiono = magic_enum::enum_name<PressedType>(action).data();
            if (keyo != nullptr && actiono != nullptr)
            {
                logger::Log::trace("KEY :: ACTION :: ", keyo, " :: ", actiono);
            }
            else
            {
                logger::Log::trace("KEY :: ACTION :: UNKNOW KEY");
            }
        }

        m_keys_inputs[key].Type = action;
        m_keys_inputs[key].justPressed = action == PressedType::PRESS;

    }

    void AikoInput::onMouseKeyPressed(Event& event)
    {
        const auto& msg = static_cast<const OnMouseKeyPressedEvent&>(event);

        const MouseButton key = static_cast<MouseButton>(msg.button);
        const PressedType action = convertToAction(msg.action);

        if (LOG_INPUT)
        {
            const char* keyo = magic_enum::enum_name<MouseButton>(key).data();
            const char* actiono = magic_enum::enum_name<PressedType>(action).data();
            if (keyo != nullptr && actiono != nullptr)
            {
                logger::Log::trace("MOUSE :: BUTTON :: ", keyo, " :: ", actiono);
            }
            else
            {
                logger::Log::trace("MOUSE :: BUTTON :: UNKNOW KEY");
            }
        }

        m_mouse_inputs[key].Type = action;
        m_mouse_inputs[key].justPressed = action == PressedType::PRESS;

    }

    void AikoInput::onMouseMoved(Event& event)
    {
        const auto& msg = static_cast<const OnMouseMoveEvent&>(event);
        vec2 newMousePosition = { msg.x, msg.y };
        m_mouseDelta = newMousePosition - m_mousePosition;
        m_mousePosition = newMousePosition;
    }

    void AikoInput::OnMouseScrollCallback(Event& event)
    {
        const auto& msg = static_cast<const OnMouseScrollCallbackEvent&>(event);
        m_mouseScrollBack = { static_cast<float>( msg.xoffset ), static_cast<float>( msg.yoffset ) };
    }

    PressedType AikoInput::convertToAction(int action)
    {
        if (action == GLFW_RELEASE)
        {
            return PressedType::RELEASE;
        }
        if (action == GLFW_PRESS)
        {
            return PressedType::PRESS;
        }
        if (action == GLFW_REPEAT)
        {
            return PressedType::REPEAT;
        }
        logger::Log::error("KEY :: ACTION :: Not Implemented");
        AIKO_ASSERT(false, "UNKNOW")
    }

}
