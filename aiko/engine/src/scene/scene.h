#pragma once

#include <aiko_types.h>

#include "scene/entity_registry.h"
#include "models/game_object.h"

namespace aiko
{

    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        void add(const AikoPtr<GameObject>& obj);
        bool remove(const GameObject* obj);

        void clear();

        vector<GameObject*> getObjects() const;

        const vector<AikoPtr<GameObject>>& objects() const { return m_objects; }

        void setActiveCamera(GameObject* obj);
        auto getActiveCamera() const -> GameObject* { return m_activeCamera; }

        EntityRegistry& registry() { return m_registry; }
        const EntityRegistry& registry() const { return m_registry; }

        template<class T>
        vector<T*> components();

    private:
        EntityRegistry m_registry;

        vector<AikoPtr<GameObject>> m_objects;
        GameObject* m_activeCamera = nullptr;

        void destroyObject(GameObject& object);

    };

    template<class T>
    vector<T*> Scene::components()
    {
        vector<T*> result;

        for (const auto& object : m_objects)
        {
            if (object == nullptr)
            {
                continue;
            }

            AikoPtr<T> component = object->getComponent<T>();
            if (component != nullptr)
            {
                result.push_back(component.get());
            }
        }

        return result;
    }

}