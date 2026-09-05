#include "aiko_input.h"

#include <aiko_types.h>
#include <time/time.h>
#include <logger/logger.h>
#include <events/events.hpp>

#include "display/display_events.hpp"

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
            PressedType type = m_keys_inputs.at(key).type;
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
            PressedType type = m_mouse_inputs.at(button).type;
            return type == PressedType::PRESS || type == PressedType::REPEAT;
        }
        return false;
    }

    void AikoInput::setCentredToScreen(bool newMouseCentred)
    {
        AIKO_ASSERT(m_window != nullptr, "Input not initialized");
        m_mouseCentred = newMouseCentred;
        if (m_mouseCentred == true)
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    bool AikoInput::getCentredToScreen() const
    {
        return m_mouseCentred;
    }

    void AikoInput::init(GLFWwindow* window)
    {
        AIKO_ASSERT(window != nullptr, "Invalid input window");
        m_window = window;
        EventSystem::it().bind<OnKeyPressedEvent>(this, &AikoInput::onKeyPressed);
        EventSystem::it().bind<OnMouseKeyPressedEvent>(this, &AikoInput::onMouseKeyPressed);
        EventSystem::it().bind<OnMouseMoveEvent>(this, &AikoInput::onMouseMoved);
        EventSystem::it().bind<OnMouseScrollEvent>(this, &AikoInput::OnMouseScrollCallback);
        glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GLFW_TRUE);
        setCentredToScreen(false);
    }

    void AikoInput::pollEvents()
    {
        glfwPollEvents();
    }

    void AikoInput::clearEvents()
    {
        for (auto it = m_keys_inputs.begin(); it != m_keys_inputs.end(); it++)
        {
            it->second.justPressed = false;
        }
        m_mouseDelta = {};
        m_mouseScrollBack = {};
    }

    void AikoInput::onKeyPressed(OnKeyPressedEvent& event)
    {
        const Key key = static_cast<Key>(event.key);
        const PressedType action = convertToAction(event.action);

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

        m_keys_inputs[key].type = action;
        m_keys_inputs[key].justPressed = action == PressedType::PRESS;

    }

    void AikoInput::onMouseKeyPressed(OnMouseKeyPressedEvent& event)
    {

        const MouseButton key = static_cast<MouseButton>(event.button);
        const PressedType action = convertToAction(event.action);

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

        m_mouse_inputs[key].type = action;
        m_mouse_inputs[key].justPressed = action == PressedType::PRESS;

    }

    void AikoInput::onMouseMoved(OnMouseMoveEvent& event)
    {
        vec2 newMousePosition = { event.x, event.y };
        m_mouseDelta = newMousePosition - m_mousePosition;
        m_mousePosition = newMousePosition;
    }

    void AikoInput::OnMouseScrollCallback(OnMouseScrollEvent& event)
    {
        m_mouseScrollBack = { static_cast<float>( event.xoffset ), static_cast<float>( event.yoffset ) };
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
