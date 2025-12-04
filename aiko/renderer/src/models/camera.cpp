#include "camera.h"

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
        mat4 view = mat4(1.0f);
        view = math::lookAt(position, target, getUp());
        return view;
    }

    mat4 Camera::getProjectionMatrix()
    {
        const auto size = DisplayManager::it().getDisplay()->getDisplaySize();
        switch (m_cameraType)
        {
        case Perspective:
        {
            return math::perspective(m_fov, (float)size.x, (float)size.y, m_near, m_far );
        }
        case Orthographic:
        {
            float aspectRatio = size.x / size.y;
            float orthoWidth = m_orthoHeight * aspectRatio;
            return math::ortho(-orthoWidth, orthoWidth, -m_orthoHeight, m_orthoHeight, m_near, m_far);
        }
        default:
            assert(false);
            return mat4(1.0f);
        }
    }

    vec3 Camera::getCameraDirection() const
    {
        return math::normalize(position - target);
    }

    vec3 Camera::getCameraRight() const
    {
        return math::normalize(math::cross(getUp(), cameraDirection));
    }

    vec3 Camera::getCameraUp() const
    {
        return math::cross(getCameraDirection(), getCameraRight());
    }

}
