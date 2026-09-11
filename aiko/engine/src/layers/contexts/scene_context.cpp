#include "scene_context.h"

#include "systems/system_connector.h"
#include "systems/scene_system.h"
#include "scene/scene.h"

namespace aiko
{

    SceneContext::SceneContext(SystemConnector& connector)
    {
        m_sceneSystem = connector.find<SceneSystem>();
        AIKO_ASSERT(m_sceneSystem != nullptr, "Required system SceneSystem not found");
    }

    GameObject* SceneContext::Instantiate(string name)
    {
        return m_sceneSystem->createGameObject(std::move(name));
    }

    GameObject* SceneContext::Instantiate(GameObject* parent, string name)
    {
        return m_sceneSystem->createGameObject(parent, std::move(name));
    }

    Color& SceneContext::clearColor()
    {
        return m_sceneSystem->getScene().clearColor();
    }

    const Color& SceneContext::clearColor() const
    {
        return m_sceneSystem->getScene().clearColor();
    }

    AmbientLight& SceneContext::ambientLight()
    {
        return m_sceneSystem->getScene().ambientLight();
    }

    const AmbientLight& SceneContext::ambientLight() const
    {
        return m_sceneSystem->getScene().ambientLight();
    }

    void SceneContext::setActiveCamera(GameObject* camera)
    {
        m_sceneSystem->getScene().setActiveCamera(camera);
    }

    GameObject* SceneContext::activeCamera()
    {
        return m_sceneSystem->getScene().getActiveCamera();
    }

    const GameObject* SceneContext::activeCamera() const
    {
        return m_sceneSystem->getScene().getActiveCamera();
    }

}
