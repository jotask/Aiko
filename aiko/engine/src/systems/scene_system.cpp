#include "scene_system.h"

#include "components/camera_component.h"
#include "display/display_manager.h"
#include "modules/render_module.h"

#include "modules/module_connector.h"

#include "scene/scene.h"
#include "models/game_object.h"

namespace aiko
{
    SceneSystem::SceneSystem()
        : m_renderModule(nullptr)
    {

    }

    void SceneSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule);
    }

    void SceneSystem::render()
    {
        SceneView view = m_sceneViewBuilder.build(m_scene);
        if(view.camera != nullptr)
        {
            m_renderModule->getRenderer().submit(view.ambientLight, view.lights);
            m_renderModule->setMainCamera(view.camera);
        }
        m_sceneRenderer.render(m_renderModule, m_scene);
    }

    void SceneSystem::update()
    {
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
        obj->aiko = aiko;

        obj->addComponent<TransforComponent>();

        m_scene.add(obj);
        return obj;
    }

    AikoPtr<GameObject> SceneSystem::createGameObject(GameObject* parent, string name)
    {
        AikoPtr<GameObject> obj = createGameObject(name);
        parent->transform().childs.push_back(&obj->transform());
        obj->transform().parent = &parent->transform();
        return obj;
    }

    void SceneSystem::destroyGameObject(const AikoPtr<GameObject>& obj)
    {
        m_scene.remove(obj.get());
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
