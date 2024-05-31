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

        void update();

        camera::CameraType getCameraType() const;
        void setCameraType(camera::CameraType);

        camera::CameraController getCameraController() const;
        void setCameraController(camera::CameraController);

        vec3 position;
        vec3 target;
        vec3 cameraDirection;
        vec3 right;
        vec3 cameraUp;

        vec3 getUp() const { return { 0.0f, 1.0f, 0.0f }; }
        float getFOV() const { return m_fov; }

        mat4 getViewMatrix();
        mat4 getProjectionMatrix();

    // private:

        bool isMainCamera = false;

        CameraSystem* cameraSystem;

        float m_fov = 0.0f;
        float radius = 3.5f;

        camera::CameraType cameraType = camera::CameraType::Perspective;
        camera::CameraController cameraControler = camera::CameraController::Static;

    };

}