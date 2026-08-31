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
        for (const auto& go : m_scene.objects())
        {
            if (go == nullptr) continue;
            if (!go->hasComponent<TransforComponent>()) continue;

            const auto transform = go->getComponent<TransforComponent>();

            if (auto cmp = go->getComponent<MeshComponent>())
            {
                m_renderSystem->render(transform->transform, *cmp);
            }

            if (auto cmp = go->getComponent<ModelComponent>())
            {
                m_renderSystem->render(transform->transform, *cmp);
            }

            if (auto cmp = go->getComponent<SpriteComponent>())
            {
                m_renderSystem->render(transform->transform, *cmp);
            }
        }
    }

    void SceneSystem::update()
    {
        AIKO_FUNCTION_PROFILE
        for (const auto& go : m_scene.objects())
        {
            if (go != nullptr)
            {
                go->update();
            }
        }
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
        for (const auto& obj : m_scene.objects())
        {
            if (obj == nullptr) continue;
            if (auto cam = obj->getComponent<CameraComponent>())
            {
                return &cam->getCamera();
            }
        }
        return nullptr;
    }

    const Camera* SceneSystem::getMainCamera() const
    {
        for (const auto& obj : m_scene.objects())
        {
            if (obj == nullptr) continue;
            if (auto cam = obj->getComponent<CameraComponent>())
            {
                return &cam->getCamera();
            }
        }
        return nullptr;
    }

    void SceneSystem::setActiveCamera(GameObject* obj)
    {
        m_scene.setActiveCamera(obj);
    }
}
