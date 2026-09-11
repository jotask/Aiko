#pragma once

#include <aiko_types.h>

#include "models/light.h"

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
