#pragma once

#include <math/math.h>
#include <input/inputs_types.h>

namespace aiko
{

    class GameObject;

    enum class UIPointerEventType
    {
        Enter,
        Exit,
        Down,
        Up,
        Click
    };

    enum class UIEventPropagation
    {
        Continue,
        Stop
    };

    struct UIPointerEvent
    {
        UIPointerEventType type = UIPointerEventType::Enter;

        GameObject* target = nullptr;

        vec2 framebufferPosition = {};

        MouseButton button = MouseButton::MOUSE_BUTTON_LEFT;
    };

    class UIPointerEventHandler
    {
    public:

        virtual ~UIPointerEventHandler() = default;

        virtual UIEventPropagation onPointerEvent(const UIPointerEvent& event) = 0;
    };

}
