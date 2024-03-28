#include "camera.h"

#include <stdexcept>

#include "systems/camera_system.h"

#include "shared/math_transform.h"

#include <GLFW/glfw3.h>

namespace aiko
{
    Camera::Camera()
    {
        position = { 0.0f, 0.0f, 3.0f };
        target = { 0.0f, 0.0f, 0.0f };
        cameraDirection = math::normalize(position - target);
        right = math::normalize(math::cross(getUp(), cameraDirection));;
        cameraUp = math::cross(cameraDirection, right);
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
        float radius = 10.0f;
        float camX = static_cast<float>(sin(glfwGetTime()) * radius);
        float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
        view = math::lookAt(vec3(camX, 0.0f, camZ), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        return view;
    }

    mat4 Camera::getProjectionMatrix()
    {
        return math::perspective( 45.0f, (float)800, (float)600, 0.1f, 100.0f);
    }

}
