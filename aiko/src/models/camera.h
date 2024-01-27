#pragma once

#include "aiko_types.h"
#include "camera_types.h"

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

    private:

        CameraSystem* cameraSystem;

        camera::CameraType cameraType = camera::CameraType::Perspective;
        camera::CameraController cameraControler = camera::CameraController::Orbit;

    };

}