#include "models/camera.h"

#include <math/math_transform.h>

#include "display/display_manager.h"

namespace aiko
{
    Camera::Camera()
        : position({ 0.0f, 0.0f, 3.0f })
        , target({0})
    {

    }

    Camera::CameraType Camera::getCameraType() const
    {
        return m_cameraType;
    }

    void Camera::setCameraType(CameraType newType)
    {
        m_cameraType = newType;
    }

    mat4 Camera::getViewMatrix() const
    {
        return math::lookAt( position, target, getUp());
    }

    mat4 Camera::getProjectionMatrix() const
    {
        const auto size = DisplayManager::it().getDisplay()->getDisplaySize();
        const float aspectRatio = static_cast<float>(size.x)/static_cast<float>(size.y);
        switch (m_cameraType)
        {
        case Perspective:
        {
            return math::perspective(m_fov, aspectRatio, m_near, m_far );
        }
        case Orthographic:
        {
            // Compute height of ortho box to match what FOV would see at distance = |camera.position.z|
            const float halfHeight = tanf(math::radians(m_fov) * 0.5f) * std::abs(position.z);
            const float halfWidth  = halfHeight * aspectRatio;
            return math::ortho(-halfWidth , halfWidth, -halfHeight , halfHeight, m_near, m_far);
        }
        default:
            AIKO_ASSERT(false, "Unknow projection");
            return mat4(1.0f);
        }
    }

    vec3 Camera::getCameraDirection() const
    {
        return math::normalize(target - position);
    }

    vec3 Camera::getCameraRight() const
    {
        return math::normalize(math::cross(getUp(), getCameraDirection()));
    }

    vec3 Camera::getCameraUp() const
    {
        return math::cross(getCameraRight(), getCameraDirection());
    }

}
