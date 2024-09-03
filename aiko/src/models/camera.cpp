#include "camera.h"

#include <stdexcept>

#include "systems/camera_system.h"
#include "shared/math_transform.h"
#include "core/libs.h"
#include "core/log.h"

namespace aiko
{
    Camera::Camera()
        : cameraSystem(nullptr)
    {
        position = { 0.0f, 0.0f, 3.0f };
        target = { 0.0f, 0.0f, 0.0f };
    }

    camera::CameraType Camera::getCameraType() const
    {
        return cameraType;
    }

    void Camera::setCameraType(camera::CameraType newType)
    {
        cameraType = newType;
        cameraSystem->setMainCamera(this);
    }

    mat4 Camera::getViewMatrix()
    {
        mat4 view = mat4(1.0f);
        view = math::lookAt(position, target, getUp());
        return view;
    }

    mat4 Camera::getProjectionMatrix()
    {
        switch (cameraType)
        {
        case camera::CameraType::Perspective:
        {
            auto size = cameraSystem->getDisplaySize();
            return math::perspective(45.0f, (float)size.x, (float)size.y, near, far );
        }
        case camera::CameraType::Orthographic:
        {
            ivec2 size = cameraSystem->getDisplaySize();
            float aspectRatio = size.x / size.y;
            float orthoWidth = orthoHeight * aspectRatio;
            return math::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, near, far);
        }
            default:
                std::exception("Not Implemented");
                assert(false);
                return mat4(1.0f);
        }
    }

    vec3 Camera::getCameraDirection()
    {
        return math::normalize(position - target);
    }

    vec3 Camera::getCameraRight()
    {
        return math::normalize(math::cross(getUp(), cameraDirection));
    }

    vec3 Camera::getCameraUp()
    {
        return math::cross(getCameraDirection(), getCameraRight());
    }

}
