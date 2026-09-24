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

    AikoInput::~AikoInput()
    {
        EventSystem::it().unbindAll(this);
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
        const auto it = m_keys_inputs.find(key);

        if (it == m_keys_inputs.end())
        {
            return false;
        }

        return it->second.justPressed;
    }

    bool AikoInput::isKeyJustReleased(Key key) const
    {
        const auto it = m_keys_inputs.find(key);

        if (it == m_keys_inputs.end())
        {
            return false;
        }

        return it->second.justReleased;
    }

    vec2 AikoInput::getMouseWindowPosition() const
    {
        return m_mouseWindowPosition ;
    }

    vec2 AikoInput::getMouseDelta() const
    {
        return m_mouseDelta;
    }

    vec2 AikoInput::getMouseScrollDelta() const
    {
        return m_mouseScrollDelta;
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

    bool AikoInput::isMouseButtonJustPressed(MouseButton button) const
    {
        const auto it = m_mouse_inputs.find(button);

        if (it == m_mouse_inputs.end())
        {
            return false;
        }

        return it->second.justPressed;
    }

    bool AikoInput::isMouseButtonJustReleased(MouseButton button) const
    {
        const auto it = m_mouse_inputs.find(button);

        if (it == m_mouse_inputs.end())
        {
            return false;
        }

        return it->second.justReleased;
    }

    void AikoInput::setMouseCaptured(bool captured)
    {
        AIKO_ASSERT(m_window != nullptr, "Input not initialized");

        m_mouseCaptured = captured;

        glfwSetInputMode(
            m_window,
            GLFW_CURSOR,
            captured
                ? GLFW_CURSOR_DISABLED
                : GLFW_CURSOR_NORMAL
        );

        double x = 0.0;
        double y = 0.0;

        glfwGetCursorPos(m_window, &x, &y);

        m_mouseWindowPosition  =
        {
            static_cast<float>(x),
            static_cast<float>(y)
        };

        m_mouseDelta = {};
    }

    bool AikoInput::isMouseCaptured() const
    {
        return m_mouseCaptured;
    }

    void AikoInput::init(GLFWwindow* window)
    {
        AIKO_ASSERT(window != nullptr, "Invalid input window");
        m_window = window;
        EventSystem::it().bind<OnKeyPressedEvent>(this, &AikoInput::onKeyInput);
        EventSystem::it().bind<OnMouseKeyPressedEvent>(this, &AikoInput::onMouseButtonInput);
        EventSystem::it().bind<OnMouseMoveEvent>(this, &AikoInput::onMouseMoved);
        EventSystem::it().bind<OnMouseScrollEvent>(this, &AikoInput::onMouseScrolled);
        setMouseCaptured(false);
    }

    void AikoInput::pollEvents()
    {
        glfwPollEvents();
    }

    void AikoInput::clearFrameState()
    {
        for (auto& [key, state] : m_keys_inputs)
        {
            state.justPressed = false;
            state.justReleased = false;
        }

        for (auto& [button, state] : m_mouse_inputs)
        {
            state.justPressed = false;
            state.justReleased = false;
        }

        m_mouseDelta = {};
        m_mouseScrollDelta = {};
    }

    void AikoInput::onKeyInput(const OnKeyPressedEvent& event)
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

        InputState& state = m_keys_inputs[key];

        state.type = action;

        if (action == PressedType::PRESS)
        {
            state.justPressed = true;
        }
        else if (action == PressedType::RELEASE)
        {
            state.justReleased = true;
        }

    }

    void AikoInput::onMouseButtonInput(const OnMouseKeyPressedEvent& event)
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

        InputState& state = m_mouse_inputs[key];

        state.type = action;

        if (action == PressedType::PRESS)
        {
            state.justPressed = true;
        }
        else if (action == PressedType::RELEASE)
        {
            state.justReleased = true;
        }

    }

    void AikoInput::onMouseMoved(const OnMouseMoveEvent& event)
    {
        const vec2 newMousePosition =
        {
            event.x,
            event.y
        };

        m_mouseDelta += newMousePosition - m_mouseWindowPosition ;
        m_mouseWindowPosition  = newMousePosition;
    }

    void AikoInput::onMouseScrolled(const OnMouseScrollEvent& event)
    {
        m_mouseScrollDelta += vec2
        {
            static_cast<float>(event.xoffset),
            static_cast<float>(event.yoffset)
        };
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
