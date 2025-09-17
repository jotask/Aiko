#pragma once

#include "aiko_types.h"
#include "types/camera_types.h"
#include "shared/math.h"

namespace aiko
{

    class CameraSystem;

    class Camera
    {
    public:
        friend class CameraSystem;

        Camera();
        ~Camera() = default;

        camera::CameraType getCameraType() const;
        void setCameraType(camera::CameraType);

        vec3 position;
        vec3 target;
        vec3 cameraDirection;
        vec3 right;
        vec3 cameraUp;

        vec3 getUp() const { return { 0.0f, 1.0f, 0.0f }; }
        float getFOV() const { return m_fov; }

        mat4 getViewMatrix();
        mat4 getProjectionMatrix();

        vec3 getCameraDirection();
        vec3 getCameraRight();
        vec3 getCameraUp();

    // private:

        bool isMainCamera = false;

        CameraSystem* cameraSystem;

        float m_fov = 45.0f;
        float m_near = 0.1f;
        float m_far = 1000.0f;
        float m_orthoHeight = 2.0f;

        camera::CameraType cameraType = camera::CameraType::Perspective;

    };

}