#include "scene_context.h"

#include "systems/system_connector.h"
#include "systems/scene_system.h"

namespace aiko
{

    SceneContext::SceneContext(SystemConnector& connector)
    {
        m_sceneSystem = connector.find<SceneSystem>();
        AIKO_ASSERT(m_sceneSystem != nullptr, "Required system SceneSystem not found");
    }

    GameObject* SceneContext::Instantiate(string name)
    {
        return m_sceneSystem->createGameObject(std::move(name)).get();
    }

    GameObject* SceneContext::Instantiate(GameObject* parent, string name)
    {
        return m_sceneSystem->createGameObject(parent, std::move(name)).get();
    }

}
