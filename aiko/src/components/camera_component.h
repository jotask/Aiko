#pragma once

#include "models/component.h"
#include "types/camera_types.h"

namespace aiko
{
    
    class Camera;
    
    class CameraComponent : public Component, public IUpdate, public IRender
    {
    public:
        CameraComponent();
        virtual ~CameraComponent() = default;
    
        camera::CameraType getCameraType() const;
        void setCameraType(camera::CameraType);
    
        camera::CameraController getCameraController() const;
        void setCameraController(camera::CameraController);

        virtual void update() override { };
        virtual void render() override { };
    
    protected:
        virtual void init() override;
    
    private:
    
        Camera* m_camera;
    
    };

}
