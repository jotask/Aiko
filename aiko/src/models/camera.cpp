#include "camera.h"

#include <stdexcept>

#include "systems/camera_system.h"
#include "shared/math_transform.h"
#include "core/libs.h"
#include "core/log.h"

namespace aiko
{
    Camera::Camera()
        : cameraSystem(nullptr)
    {
        position = { 0.0f, 0.0f, 3.0f };
        target = { 0.0f, 0.0f, 0.0f };
        cameraDirection = math::normalize(position - target);
        right = math::normalize(math::cross(getUp(), cameraDirection));;
        cameraUp = math::cross(cameraDirection, right);
    }

    void Camera::update()
    {
        switch (cameraControler)
        {
        case camera::CameraController::Orbit:
        {
            float camX = static_cast<float>(sin(glfwGetTime()) * radius);
            float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
            position = { camX, position.y, camZ };
        }
        break;
        case camera::CameraController::Fly:
        {

        }
        break;
        case camera::CameraController::Drag:
        {

        }
        break;
        case camera::CameraController::Static:
        {

        }
        break;
        default:
            Log::error( "CAMERA :: UPDATE :: UNKNOW CONTROLLER");
            break;
        }
        
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

    mat4 Camera::getViewMatrix()
    {
        mat4 view = mat4(1.0f);
        view = math::lookAt(position, target, getUp());
        return view;
    }

    mat4 Camera::getProjectionMatrix()
    {
        // TODO Get display size
        switch (cameraType)
            {
            case camera::CameraType::Perspective:
            {
                auto size = cameraSystem->getDisplaySize();
                return math::perspective( 45.0f, (float)size.x, (float)size.y, 0.1f, 100.0f);
            }
            default:
                std::exception("Not Implemented");
                return mat4(1.0f);
        }
    }

}
