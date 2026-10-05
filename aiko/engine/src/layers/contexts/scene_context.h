#pragma once

#include "components/camera_component.h"
#include "models/light.h"

#include <aiko_types.h>

namespace aiko
{

    class SystemConnector;
    class SceneSystem;
    class GameObject;

    class SceneContext
    {
    public:

        GameObject* Instantiate(string name);
        GameObject* Instantiate(GameObject* parent, string name);

        CameraComponent* createCamera(camera::CameraController controller = camera::CameraController::Static, Camera::CameraType type = Camera::CameraType::Perspective);

        Color& clearColor();
        const Color& clearColor() const;

        AmbientLight& ambientLight();
        const AmbientLight& ambientLight() const;

        void setActiveCamera(GameObject* camera);
        GameObject* activeCamera();
        const GameObject* activeCamera() const;

    private:
        friend class LayerContext;

        explicit SceneContext(SystemConnector& connector);

        SceneSystem* m_sceneSystem = nullptr;
    };
}
