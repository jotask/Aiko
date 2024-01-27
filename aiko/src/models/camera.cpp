#include "camera.h"

#include <stdexcept>

#include "systems/camera_system.h"

namespace aiko
{

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
