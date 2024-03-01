#include "camera_system.h"

#include <core/libs.h>
#include "modules/module_connector.h"

#include "modules/renderer/render_module.h"
#include "modules/scene_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "models/camera_types.h"
#include "models/camera.h"

namespace aiko
{
    
    void CameraSystem::setCameraType(camera::CameraType cameraType)
    {
        // This needs to delegate to module
        switch (cameraType)
        {
        case camera::CameraType::Orthographic:
    
            break;
        case camera::CameraType::Perspective:
    
            break;
        default:
            throw std::exception();
            break;
        }
    }
    
    void CameraSystem::setCameraController(camera::CameraController cameraController)
    {
        // This needs to delegate to module
        switch (cameraController)
        {
        case camera::CameraController::Orbit:
    
            break;
        case camera::CameraController::Fly:
    
            break;
        case camera::CameraController::Drag:
    
            break;
        default:
            throw std::exception();
            break;
        }
    }
    
    void CameraSystem::setMainCamera(Camera* camera)
    {
        setCameraType(camera->cameraType);
        setCameraController(camera->cameraControler);
    }

    Camera* CameraSystem::getMainCamera()
    {
        return nullptr;
    }

    ivec2 CameraSystem::getDisplaySize()
    {
        return m_renderModule->getDisplaySize();
    }
    
    Camera* CameraSystem::createCamera(bool setMain)
    {  
        Camera& cam = m_cameras.emplace_back();
        cam.cameraSystem = this;
        if (setMain == true)
        {
            setMainCamera(&cam);
        }
        return &cam;
    }
    
    void CameraSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        m_renderModule = moduleConnector->find<RenderModule>();
        m_sceneModule = moduleConnector->find<SceneModule>();
    }
    
    void CameraSystem::init()
    {
    
    }
    
    void CameraSystem::update()
    {
    
    }
    
    void CameraSystem::render()
    {
        
    }
    

}
