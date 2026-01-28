#pragma once

#include <models/camera.h>

#include "models/component.h"
#include "types/camera_controller_types.h"

namespace aiko
{
    
    class Camera;
    class InputSystem;
    class CameraSystem;
    
    class CameraComponent : public Component, public IUpdate, public IRender3D
    {
    public:
        CameraComponent();
        CameraComponent(camera::CameraController, Camera::CameraType type = Camera::CameraType::Perspective);
        virtual ~CameraComponent() = default;
    
        Camera::CameraType getCameraType() const;
        void setCameraType(Camera::CameraType);
    
        camera::CameraController getCameraController() const;
        void setCameraController(camera::CameraController);

        virtual void update() override;
        virtual void render() override { };

        float& radius() { return m_radius; }
        float& speed() { return m_speed; }

        Camera& getCamera() { return *m_camera; }
    
    protected:
        virtual void init() override;
    
    private:

        InputSystem* m_inputSystem;
        CameraSystem* m_cameraSystem;
        Camera::CameraType m_type;

        bool isMainCamera;

        // Orbit
        float m_radius = 3.5f;

        // Fly
        float m_speed = 3.5f;

        Camera* m_camera;
        camera::CameraController cameraControler = camera::CameraController::Static;
    
    };

}
