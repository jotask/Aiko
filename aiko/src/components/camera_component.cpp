#include "components/camera_component.h"

#include "systems/camera_system.h"
#include "models/camera.h"

CameraComponent::CameraComponent()
    : Component("Camera")
    , m_camera(nullptr)
{
}

camera::CameraType CameraComponent::getCameraType() const
{
    return m_camera->getCameraType();
}

void CameraComponent::setCameraType(camera::CameraType newType)
{
    m_camera->setCameraType(newType);
}

camera::CameraController CameraComponent::getCameraController() const
{
    return m_camera->getCameraController();
}

void CameraComponent::setCameraController(camera::CameraController newController)
{
    m_camera->setCameraController(newController);
}

void CameraComponent::init()
{
    auto system = gameobject->getSystem<CameraSystem>();
    m_camera = system->createCamera(true);
}
