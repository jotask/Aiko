#pragma once

#include <math/math.h>

namespace aiko
{

    class Camera
    {
    public:

        enum CameraType { Perspective, Orthographic };

        Camera();
        ~Camera() = default;

        vec3 position;
        vec3 target;

        CameraType getCameraType() const;
        void setCameraType(CameraType);

        vec3 getUp() const { return { 0.0f, 1.0f, 0.0f }; }
        float getFOV() const { return m_fov; }

        mat4 getViewMatrix() const;
        mat4 getProjectionMatrix() const;

        vec3 getCameraDirection() const;
        vec3 getCameraRight() const;
        vec3 getCameraUp() const;

    // private:

        CameraType m_cameraType;

        float m_fov = 45.0f;
        float m_near = 0.1f;
        float m_far = 1000.0f;
        float m_orthoHeight = 2.0f;

    };

}