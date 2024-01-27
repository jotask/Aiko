#pragma once

#include <glm/glm.hpp>

#include "models/component.h"
#include "models/camera_types.h"

namespace aiko
{
    
    class Camera;
    
    class CameraComponent : public Component
    {
    public:
        CameraComponent();
        virtual ~CameraComponent() = default;
    
        camera::CameraType getCameraType() const;
        void setCameraType(camera::CameraType);
    
        camera::CameraController getCameraController() const;
        void setCameraController(camera::CameraController);
    
    protected:
        virtual void init() override;
    
    private:
    
        aiko::AikoPtr<Camera> m_camera;
    
    };

}
