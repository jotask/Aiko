#include "entity_component_system.h"

#include "modules/module_connector.h"

#include "components/transform_component.h"
#include "models/game_object.h"

namespace aiko
{
    
    AikoPtr<GameObject> EntityComponentSystem::createGameObject(string name)
    {
        auto obj = std::make_shared<GameObject>();

        obj->m_entity = createEntity();
        obj->setName(name);
        obj->aiko = aiko;

        auto trans = obj->addComponent<TransforComponent>();

        m_scene.add(obj);

        return obj;
    }

    AikoPtr<GameObject> EntityComponentSystem::createGameObject(GameObject* parent, string name)
    {
        AikoPtr<GameObject> obj = createGameObject(name);
        parent->transform().childs.push_back(&obj->transform());
        obj->transform().parent = &parent->transform();
        return obj;
    }

    void EntityComponentSystem::destroyGameObject(GameObject* obj)
    {
        m_scene.remove(obj);
    }
    
    void EntityComponentSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {

    }
    
    void EntityComponentSystem::init()
    {
    }
    
    void EntityComponentSystem::update()
    {
        for (const auto& go : m_scene.objects())
        {
            if (go != nullptr)
            {
                go->update();
            }
        }
    }
    
    void EntityComponentSystem::render()
    {
        m_sceneRenderer.render(m_scene);
    }

    std::vector<GameObject*> EntityComponentSystem::getObjects()
    {
        return m_scene.getObjects();
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
