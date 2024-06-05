#include "components/camera_component.h"

#include "systems/camera_system.h"
#include "models/camera.h"
#include "models/time.h"

namespace aiko
{

    CameraComponent::CameraComponent()
        : CameraComponent(camera::CameraController::Static)
    {
    }

    CameraComponent::CameraComponent(camera::CameraController controller)
        : Component("Camera")
        , m_camera(nullptr)
        , cameraControler(controller)
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
        return cameraControler;
    }
    
    void CameraComponent::setCameraController(camera::CameraController newController)
    {
        cameraControler = newController;
    }

    void CameraComponent::update()
    {
        switch (cameraControler)
        {
        case camera::CameraController::Orbit:
        {
            auto timer = aiko::Time::it().secondSinceStart();
            float camX = static_cast<float>(sin(timer) * m_radius);
            float camZ = static_cast<float>(cos(timer) * m_radius);
            m_camera->position = { camX, m_camera->position.y, camZ };
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
            Log::error("CAMERA :: UPDATE :: UNKNOW CONTROLLER");
            break;
        }
    }
    
    void CameraComponent::init()
    {
        auto system = gameobject->getSystem<CameraSystem>();
        m_camera = system->createCamera(true);
    }

}
