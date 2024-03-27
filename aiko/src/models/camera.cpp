#include "camera.h"

#include <stdexcept>

#include "systems/camera_system.h"

#include "shared/math_transform.h"

namespace aiko
{
    Camera::Camera()
    {
        position = { 0.0f, 0.0f, 3.0f };
        target = { 0.0f, 0.0f, 0.0f };
        cameraDirection = math::normalize(position - target);
        right ;
        cameraUp ;
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

    camera::CameraController Camera::getCameraController() const
    {
        return cameraControler;
    }

    void Camera::setCameraController(camera::CameraController newController)
    {
        cameraControler = newController;
        cameraSystem->setMainCamera(this);
    }

}
