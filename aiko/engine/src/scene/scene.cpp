#include "scene.h"

#include "models/game_object.h"

namespace aiko
{
    void Scene::add(const AikoPtr<GameObject>& obj)
    {
        m_objects.push_back(obj);
    }

    bool Scene::remove(const GameObject* obj)
    {
        if (obj == nullptr)
        {
            return false;
        }

        if (m_activeCamera == obj)
        {
            m_activeCamera = nullptr;
        }

        auto it = std::find_if( m_objects.begin(), m_objects.end() , [obj](const AikoPtr<GameObject>& go)
        {
            return go != nullptr && ( go.get() == obj);
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

        object->dispose();

        if (m_registry.valid(object->m_entity))
        {
            m_registry.destroy(object->m_entity);
        }

        object->m_entity = {};

        m_objects.erase(it);

        return true;
    }

    void Scene::clear()
    {
        for (const auto& object: m_objects)
        {
            if (object == nullptr)
            {
                continue;
            }

            object->dispose();

            if (m_registry.valid(object->m_entity))
            {
                m_registry.destroy(object->m_entity);
            }

            object->m_entity = {};
        }

        m_objects.clear();
        m_activeCamera = nullptr;
    }

    vector<GameObject*> Scene::getObjects() const
    {
        vector<GameObject*> objs;
        objs.reserve(m_objects.size());
        for (const auto& obj: m_objects)
        {
            objs.push_back(obj.get());
        }
        return objs;
    }

    void Scene::setActiveCamera(GameObject* obj)
    {
        // Allow nullptr to clear active camera
        if (obj == nullptr)
        {
            m_activeCamera = nullptr;
            return;
        }

        // Only accept objects that exist in this scene
        bool exists = std::any_of(m_objects.begin(), m_objects.end(), [obj](const AikoPtr<GameObject>& go) { return go != nullptr && go.get() == obj; });

        if (exists)
        {
            m_activeCamera = obj;
        }
    }
}
