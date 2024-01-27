#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/camera.h"
#include "models/camera_types.h"

namespace aiko
{
    
    class SceneModule;
    class Camera;
    
    class CameraSystem : public BaseSystem
    {
    public:
    
        CameraSystem() = default;
        virtual ~CameraSystem() = default;
    
        std::shared_ptr<Camera> createCamera(bool setMain = false);
        void setMainCamera(Camera* camera);
    
    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) override;
    
        virtual void init() override;
    
        virtual void update() override;
    
        virtual void render() override;
    
    private:
    
        aiko::AikoPtr<SceneModule> m_sceneModule;
    
        void setCameraType(camera::CameraType cameraType);
        void setCameraController(camera::CameraController cameraController);
    
    };

}
