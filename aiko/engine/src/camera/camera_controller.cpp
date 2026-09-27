#include "camera_controller.h"

#include "models/camera.h"
#include "systems/input_system.h"

#include <math/math.h>

#include <cmath>

namespace aiko::camera
{

    void updateDrag(Camera& camera, const InputSystem& input)
    {
        DragInput dragInput;
        dragInput.mouseDelta = input.getMouseDelta();
        dragInput.scrollDelta = input.getMouseScrollDelta();
        dragInput.rightMouse = input.isMouseButtonPressed(MouseButton::MOUSE_BUTTON_RIGHT);
        dragInput.middleMouse = input.isMouseButtonPressed(MouseButton::MOUSE_BUTTON_MIDDLE);
        updateDrag(camera, dragInput);
    }

    void updateDrag(Camera& camera, const DragInput& input)
    {
        if (input.rightMouse || (input.alt && input.leftMouse))
        {
            const vec2 mouseDelta = input.mouseDelta;

            constexpr float sensitivity = 0.002f;

            const vec3 direction = camera.position - camera.target;

            const float angleX = mouseDelta.x * sensitivity;

            const float angleY = mouseDelta.y * sensitivity;

            const float cosAngleX = std::cos(angleX);
            const float sinAngleX = std::sin(angleX);

            const vec3 newDirX(
                cosAngleX * direction.x - sinAngleX * direction.z,
                direction.y,
                sinAngleX * direction.x + cosAngleX * direction.z);

            const vec3 right = math::normalize(math::cross(direction, camera.getUp()));

            const float cosAngleY = std::cos(angleY);
            const float sinAngleY = std::sin(angleY);

            const vec3 newDirY = math::normalize(cosAngleY * newDirX + sinAngleY * camera.getUp());

            camera.position = camera.target + newDirY * math::length(direction);
        }

        if (input.middleMouse)
        {
            constexpr float panSpeed = 0.01f;

            const vec2 mouseDelta = input.mouseDelta;

            const vec3 right = math::normalize(math::cross(camera.getCameraDirection(), camera.getUp()));

            const vec3 upMove = camera.getUp() * (-mouseDelta.y * panSpeed);

            const vec3 rightMove = right * (mouseDelta.x * panSpeed);

            camera.position += rightMove + upMove;
            camera.target += rightMove + upMove;
        }

        constexpr float epsilon = 1e-6f;

        if (std::fabs(input.scrollDelta.y) > epsilon)
        {
            constexpr float zoomSpeed = 0.5f;

            const vec3 direction = math::normalize(camera.target - camera.position);

            const float amount = input.scrollDelta.y * zoomSpeed;

            camera.position += direction * amount;
        }
    }

}
