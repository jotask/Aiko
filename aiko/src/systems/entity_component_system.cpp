#include "entity_component_system.h"

#include "modules/module_connector.h"

#include "components/transform_component.h"
#include "components/component_renderer.h"
#include "models/game_object.h"

namespace aiko
{
    
    aiko::AikoPtr<GameObject> EntityComponentSystem::createGameObject(std::string name)
    {
        m_gameObjects.emplace_back(std::make_shared<GameObject>());
        aiko::AikoPtr<GameObject> obj = m_gameObjects.back();
        obj->m_entity = createEntity();
        obj->setName(name);
        obj->aiko = aiko;
        auto trans = obj->addComponent<Transform>();
        return obj;
    }
    
    void EntityComponentSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        // BIND_MODULE_REQUIRED(SceneModule, moduleConnector, m_sceneModule)
    }
    
    void EntityComponentSystem::init()
    {
    }
    
    void EntityComponentSystem::update()
    {
        for (auto& go : m_gameObjects) go->update();
    }
    
    void EntityComponentSystem::render()
    {
        for (auto& go : m_gameObjects) go->render();
    }

    std::vector<GameObject*> EntityComponentSystem::getObjects()
    {
        std::vector<GameObject*> objs;
        for (auto& obj : m_gameObjects)
        {
            objs.push_back(obj.get());
        }
        return objs;
    }

    template<class T>
    inline void EntityComponentSystem::onComponentAdded(GameObject* obj, T* c)
    {
        int a = 0;
    }

    template<class T>
    inline void EntityComponentSystem::onComponentRemoved(GameObject* obj, T* c)
    {
        int a = 0;
    }

    SceneObject EntityComponentSystem::createEntity()
    {
        SceneObject so{ };
        so.ecs = this;
        return so;
    }
    

}
