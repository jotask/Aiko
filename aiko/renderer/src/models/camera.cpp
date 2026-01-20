#include "camera.h"

#include <math/math_transform.h>

#include "display/display_manager.h"

#include <bx/math.h>
#include <bgfx/bgfx.h>

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
        // FIXME: Extract this so it's renderer specific
        mat4 view;
        const auto up = getUp();
        bx::mtxLookAt(
            view.data(),
            bx::Vec3{ position.x, position.y, position.z },
            bx::Vec3{ target.x,   target.y,   target.z   },
            bx::Vec3{ up.x, up.y, up.z }
        );
        return view;
    }

    mat4 Camera::getProjectionMatrix() const
    {

        // FIXME: Extract this so it's renderer specific

        const auto size = DisplayManager::it().getDisplay()->getDisplaySize();
        const ::bgfx::Caps* caps = ::bgfx::getCaps();
        switch (m_cameraType)
        {
        case Perspective:
        {
            mat4 result;
            const float aspectRatio = static_cast<float>(size.x)/static_cast<float>(size.y);
            bx::mtxProj(result.data(), 60.0f, aspectRatio,  m_near, m_far, caps->homogeneousDepth);
            return result;
        }
        case Orthographic:
        {
            mat4 result;
            bx::mtxOrtho(
                  result.data()
                , 0.0f
                , static_cast<float>(size.x)
                , static_cast<float>(size.y)
                , 0.0f
                , m_near
                , m_far
                , 0.0f
                , caps->homogeneousDepth
                );
            return result;
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
