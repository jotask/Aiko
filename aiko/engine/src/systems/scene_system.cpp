#include "scene_system.h"

#include "components/camera_component.h"
#include "display/display_manager.h"
#include <intrumentor/profiler.h>
#include "modules/render_module.h"

#include "modules/module_connector.h"
#include "systems/render_system.h"
#include "systems/system_connector.h"

#include "scene/scene.h"
#include "models/game_object.h"

namespace aiko
{
    SceneSystem::SceneSystem()
        : m_renderModule(nullptr)
        , m_renderSystem(nullptr)
    {

    }

    void SceneSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule);
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem);
    }

    void SceneSystem::render()
    {
        AIKO_FUNCTION_PROFILE
        SceneView view = m_sceneViewBuilder.build(m_scene);
        if(view.camera != nullptr)
        {
            m_renderModule->getRenderer().submit(view.ambientLight, view.lights);
            m_renderModule->setMainCamera(view.camera);
        }

        for (MeshComponent* component : m_scene.components<MeshComponent>())
        {
            GameObject* gameObject = component->getGameObject();
            AIKO_ASSERT(gameObject != nullptr, "MeshComponent is not attached to a GameObject");

            m_renderSystem->render(gameObject->transform(), *component);
        }

        for (ModelComponent* component : m_scene.components<ModelComponent>())
        {
            GameObject* gameObject = component->getGameObject();
            AIKO_ASSERT(gameObject != nullptr, "ModelComponent is not attached to a GameObject");

            m_renderSystem->render(gameObject->transform(), *component);
        }

        for (SpriteComponent* component : m_scene.components<SpriteComponent>())
        {
            GameObject* gameObject = component->getGameObject();
            AIKO_ASSERT(gameObject != nullptr, "SpriteComponent is not attached to a GameObject");

            m_renderSystem->render(gameObject->transform(), *component);
        }
    }

    void SceneSystem::dispose()
    {
        m_scene.clear();
    }

    AikoPtr<GameObject> SceneSystem::createGameObject(string name)
    {
        auto obj = std::make_shared<GameObject>();
        obj->setName(name);
        m_scene.add(obj);
        return obj;
    }

    AikoPtr<GameObject> SceneSystem::createGameObject(GameObject* parent, string name)
    {
        AikoPtr<GameObject> obj = createGameObject(name);
        obj->transform().setParent(&parent->transform());
        return obj;
    }

    void SceneSystem::destroyGameObject(const AikoPtr<GameObject>& obj)
    {
        if (obj != nullptr)
        {
            m_scene.remove(obj.get());
        }
    }

    Scene& SceneSystem::getScene()
    {
        return m_scene;
    }

    const Scene& SceneSystem::getScene() const
    {
        return m_scene;
    }

    Camera* SceneSystem::getMainCamera()
    {
        const auto cameras = m_scene.components<CameraComponent>();
        if (cameras.empty())
        {
            return nullptr;
        }
        return &cameras.front()->getCamera();
    }

    const Camera* SceneSystem::getMainCamera() const
    {
        const auto cameras = m_scene.components<CameraComponent>();
        if (cameras.empty())
        {
            return nullptr;
        }
        return &cameras.front()->getCamera();
    }

    void SceneSystem::setActiveCamera(GameObject* obj)
    {
        m_scene.setActiveCamera(obj);
    }
}
