#pragma once

#include <aiko_types.h>

#include "models/game_object.h"

namespace aiko
{

    class EntityRegistry;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        void add(const AikoPtr<GameObject>& obj);
        bool remove(const GameObject* obj);

        void clear();

        vector<GameObject*> getObjects();
        vector<const GameObject*> getObjects() const;

        void setActiveCamera(GameObject* obj);
        GameObject* getActiveCamera() { return m_activeCamera; }
        const GameObject* getActiveCamera() const { return m_activeCamera; }

        template<class T>
        vector<T*> components();

    private:
        AikoUPtr<EntityRegistry> m_registry;

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

            for (T* component : object->getComponents<T>())
            {
                result.push_back(component);
            }
        }

        return result;
    }

}