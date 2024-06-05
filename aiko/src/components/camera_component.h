#pragma once

#include "models/component.h"
#include "types/camera_types.h"

namespace aiko
{
    
    class Camera;
    
    class CameraComponent : public Component, public IUpdate, public IRender3D
    {
    public:
        CameraComponent();
        CameraComponent(camera::CameraController);
        virtual ~CameraComponent() = default;
    
        camera::CameraType getCameraType() const;
        void setCameraType(camera::CameraType);
    
        camera::CameraController getCameraController() const;
        void setCameraController(camera::CameraController);

        virtual void update() override;
        virtual void render() override { };

        float& radius() { return m_radius; }

        Camera* getCamera() { return m_camera; }
    
    protected:
        virtual void init() override;
    
    private:

        float m_radius = 3.5f;

        Camera* m_camera;
        camera::CameraController cameraControler = camera::CameraController::Static;
    
    };

}
