#pragma once

#include <memory>
#include <vector>

#include <aiko_types.h>
#include <math/math.h>
#include <core/uuid.h>
#include <models/camera.h>

#include "systems/base_system.h"
#include "models/game_object.h"
#include "types/camera_controller_types.h"

namespace aiko
{

    class RenderModule;
    class Camera;
    
    class CameraSystem : public BaseSystem
    {
    public:
    
        using CameraPtr = std::shared_ptr<Camera>;

        CameraSystem() = default;
        virtual ~CameraSystem() = default;
    
        Camera* createCamera(bool setMain = false);
        void setMainCamera(Camera* camera);
        Camera* getMainCamera();
        ivec2 getDisplaySize();
    
    protected:
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    private:

        RenderModule* m_renderModule;
        std::vector<Camera> m_cameras;
    
    };

}
