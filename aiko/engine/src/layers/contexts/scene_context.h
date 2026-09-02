#pragma once

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

    private:
        friend class LayerContext;

        explicit SceneContext(SystemConnector& connector);

        SceneSystem* m_sceneSystem = nullptr;
    };
}
