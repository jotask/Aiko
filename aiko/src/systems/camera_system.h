#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/base_system.h"
#include "models/game_object.h"
#include "models/camera.h"
#include "types/camera_types.h"
#include "shared/math.h"

namespace aiko
{

    class RenderModule;
    class SceneModule;
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
    
        virtual void init() override;
    
        virtual void update() override;
    
        virtual void render() override;
    
    private:

        RenderModule* m_renderModule;
        SceneModule* m_sceneModule;

        std::vector<Camera> m_cameras;
    
        void setCameraType(camera::CameraType cameraType);
        void setCameraController(camera::CameraController cameraController);
    
    };

}
