#pragma once

#include <models/camera.h>

#include "models/component.h"
#include "types/camera_controller_types.h"

namespace aiko
{
    
    class Camera;
    
    class CameraComponent : public Component
    {
    public:
        CameraComponent();
        CameraComponent(camera::CameraController, Camera::CameraType type = Camera::CameraType::Perspective);
        virtual ~CameraComponent() = default;
    
        Camera::CameraType getCameraType() const;
        void setCameraType(Camera::CameraType);
    
        camera::CameraController getCameraController() const;
        void setCameraController(camera::CameraController);

        float& radius() { return m_radius; }
        float& speed() { return m_speed; }

        const Camera& getCamera() const { return m_camera; }
        Camera& getCamera() { return m_camera; }

        bool isMain() const { return isMainCamera; }
    
    protected:
        virtual void init() override;
    
    private:

        Camera::CameraType m_type;

        bool isMainCamera = true;

        // Orbit
        float m_radius = 3.5f;

        // Fly
        float m_speed = 3.5f;

        Camera m_camera;
        camera::CameraController cameraControler = camera::CameraController::Static;
    
    };

}
