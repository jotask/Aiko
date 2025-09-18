#include "entity_component_system.h"

#include "modules/module_connector.h"

#include "components/transform_component.h"
#include "models/game_object.h"

namespace aiko
{
    
    aiko::AikoPtr<GameObject> EntityComponentSystem::createGameObject(string name)
    {
        auto obj = std::make_shared<GameObject>();
        m_gameObjects.emplace_back(obj);
        obj->m_entity = createEntity();
        obj->setName(name);
        obj->aiko = aiko;
        auto trans = obj->addComponent<Transform>();
        return obj;
    }

    aiko::AikoPtr<GameObject> EntityComponentSystem::createGameObject(GameObject* parent, string name)
    {
        aiko::AikoPtr<GameObject> obj = createGameObject(name);
        parent->transform()->childs.push_back(obj->transform().get());
        obj->transform()->parent = parent->transform().get();
        return obj;
    }

    void EntityComponentSystem::destroyGameObject(GameObject* obj)
    {
        auto found = std::find_if(m_gameObjects.begin(), m_gameObjects.end(), [obj](const aiko::AikoPtr<GameObject>& gameObject)
        {
            return gameObject->uuid() == obj->uuid();
        });

        if (found != m_gameObjects.end())
        {
            m_gameObjects.erase(found);
        }
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
