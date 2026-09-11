#include "components/camera_component.h"

#include "models/camera.h"

namespace aiko
{

    CameraComponent::CameraComponent()
        : CameraComponent(camera::CameraController::Static)
    {
    }

    CameraComponent::CameraComponent(camera::CameraController controller, Camera::CameraType type)
        : Component("Camera")
        , cameraControler(controller)
        , m_camera({})
        , m_type(type)
    {

    }
    
    Camera::CameraType CameraComponent::getCameraType() const
    {
        return m_camera.getCameraType();
    }
    
    void CameraComponent::setCameraType(Camera::CameraType newType)
    {
        m_camera.setCameraType(newType);
    }
    
    camera::CameraController CameraComponent::getCameraController() const
    {
        return cameraControler;
    }
    
    void CameraComponent::setCameraController(camera::CameraController newController)
    {
        cameraControler = newController;
    }
    
    void CameraComponent::init()
    {
        m_camera.setCameraType(m_type);
    }

}
