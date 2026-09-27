#pragma once

#include <aiko_types.h>

namespace aiko
{
    class Camera;
    class InputSystem;
}

namespace aiko::camera
{

    struct DragInput
    {
        vec2 mouseDelta = {};
        vec2 scrollDelta = {};

        bool leftMouse = false;
        bool rightMouse = false;
        bool middleMouse = false;
        bool alt = false;
    };

    void updateDrag(Camera& camera, const DragInput& input);
    void updateDrag(Camera& camera, const InputSystem& input);

}