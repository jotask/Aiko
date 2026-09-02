#include "scene.h"

#include "models/game_object.h"
#include "scene/entity_registry.h"

#include <algorithm>

namespace aiko
{
    Scene::Scene()
        : m_registry(std::make_unique<EntityRegistry>())
    {

    }

    Scene::~Scene() = default;

    void Scene::add(const AikoPtr<GameObject>& obj)
    {
        if (obj == nullptr)
        {
            return;
        }
        AIKO_ASSERT(obj->m_scene == nullptr, "GameObject is already attached to a scene");
        AIKO_ASSERT(obj->m_entity.valid() == false, "GameObject is already attached to a scene");
        obj->m_scene = this;
        obj->m_entity = m_registry->create();
        registerObjectComponents(*obj);
        m_objects.push_back(obj);
    }

    bool Scene::remove(const GameObject* obj)
    {
        if (obj == nullptr)
        {
            return false;
        }

        auto it = std::find_if(m_objects.begin(), m_objects.end(), [obj](const AikoPtr<GameObject>& go)
        {
            return go != nullptr && go.get() == obj;
        });

        if (it == m_objects.end())
        {
            return false;
        }

        AikoPtr<GameObject>& object = *it;

        if (m_activeCamera == object.get())
        {
            m_activeCamera = nullptr;
        }

        destroyObject(*object);

        m_objects.erase(it);

        return true;
    }

    void Scene::clear()
    {
        for (const auto& object : m_objects)
        {
            if (object != nullptr)
            {
                destroyObject(*object);
            }
        }

        m_objects.clear();
        m_componentIndex.clear();
        m_activeCamera = nullptr;
    }

    vector<GameObject*> Scene::getObjects()
    {
        vector<GameObject*> objs;
        objs.reserve(m_objects.size());
        for (const auto& obj: m_objects)
        {
            objs.push_back(obj.get());
        }
        return objs;
    }

    vector<const GameObject*> Scene::getObjects() const
    {
        vector<const GameObject*> objs;
        objs.reserve(m_objects.size());
        for (const auto& obj: m_objects)
        {
            objs.push_back(obj.get());
        }
        return objs;
    }

    void Scene::setActiveCamera(GameObject* obj)
    {
        if (obj == nullptr)
        {
            m_activeCamera = nullptr;
            return;
        }
        bool exists = std::any_of(m_objects.begin(), m_objects.end(), [obj](const AikoPtr<GameObject>& go) { return go != nullptr && go.get() == obj; });
        if (exists)
        {
            m_activeCamera = obj;
        }
    }

    void Scene::destroyObject(GameObject& object)
    {
        Transform& transform = object.transform();
        transform.clearChildren();
        transform.clearParent();

        unregisterObjectComponents(object);

        object.dispose();

        if (m_registry->valid(object.m_entity))
        {
            m_registry->destroy(object.m_entity);
        }

        object.m_entity = {};
        object.m_scene = nullptr;
    }

    void Scene::registerComponent(Component* component, std::type_index type)
    {
        if (component == nullptr)
        {
            return;
        }

        m_componentIndex[type].push_back(component);
    }

    void Scene::unregisterComponent(Component* component, std::type_index type)
    {
        if (component == nullptr)
        {
            return;
        }

        auto it = m_componentIndex.find(type);
        if (it == m_componentIndex.end())
        {
            return;
        }

        auto& bucket = it->second;
        std::erase(bucket, component);

        if (bucket.empty())
        {
            m_componentIndex.erase(it);
        }
    }

    void Scene::registerObjectComponents(GameObject& object)
    {
        for (const auto& [type, components] : object.m_componentIndex)
        {
            for (Component* component : components)
            {
                registerComponent(component, type);
            }
        }
    }

    void Scene::unregisterObjectComponents(GameObject& object)
    {
        for (const auto& [type, components] : object.m_componentIndex)
        {
            for (Component* component : components)
            {
                unregisterComponent(component, type);
            }
        }
    }

}
