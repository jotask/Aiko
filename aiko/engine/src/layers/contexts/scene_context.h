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

        AmbientLight& ambientLight();
        const AmbientLight& ambientLight() const;

    private:
        friend class LayerContext;

        explicit SceneContext(SystemConnector& connector);

        SceneSystem* m_sceneSystem = nullptr;
    };
}
