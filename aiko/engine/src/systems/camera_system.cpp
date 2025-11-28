#include "camera_system.h"

#include "modules/module_connector.h"

#include "modules/render/render_module.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "types/camera_types.h"
#include "models/camera.h"

namespace aiko
{

    void CameraSystem::setMainCamera(Camera* camera)
    {
        for (auto& c : m_cameras)
        {
            c.isMainCamera = false;
        }
        camera->isMainCamera = true;
    }

    Camera* CameraSystem::getMainCamera()
    {
        if (m_cameras.size() == 0)
        {
            return nullptr;
        }
        return &m_cameras[0];
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
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule)
    }

}
