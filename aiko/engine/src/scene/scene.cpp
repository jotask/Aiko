#include "scene.h"

#include "models/game_object.h"

namespace aiko
{
    void Scene:: add(const AikoPtr<GameObject>& obj)
    {
        m_objects.push_back(obj);
    }

    bool Scene::remove(const GameObject* obj)
    {
        if (obj == nullptr)
        {
            return false;
        }

        auto it = std::find_if( m_objects.begin(), m_objects.end() , [obj](const AikoPtr<GameObject>& go)
        {
            return go != nullptr && ( go.get() == obj);
        });

        if (it == m_objects.end())
        {
            return false;
        }
        m_objects.erase(it);
        return true;
    }

    void Scene::clear()
    {
        m_objects.clear();
    }

    std::vector<GameObject*> Scene::getObjects() const
    {
        std::vector<GameObject*> objs;
        objs.reserve(m_objects.size());
        for (const auto& obj : m_objects)
        {
            objs.push_back(obj.get());
        }
        return objs;
    }
}
