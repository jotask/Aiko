#pragma once

#include "aiko_types.h"
#include "camera_types.h"
#include "shared/math.h"

namespace aiko
{

    class CameraSystem;

    class Camera
    {
    public:
        friend class CameraSystem;

        Camera() = default;
        ~Camera() = default;

        camera::CameraType getCameraType() const;
        void setCameraType(camera::CameraType);

        camera::CameraController getCameraController() const;
        void setCameraController(camera::CameraController);

        vec3 position = { 30.0f, 20.0f, 30.0f };
        vec3 target = { 0.0f, 0.0f, 0.0f };

        vec3 getUp() const { return { 0.0f, 1.0f, 0.0f }; }
        float getFOV() const { return m_fov; }

    private:

        CameraSystem* cameraSystem;

        float m_fov = 0.0f;

        camera::CameraType cameraType = camera::CameraType::Perspective;
        camera::CameraController cameraControler = camera::CameraController::Orbit;

    };

}