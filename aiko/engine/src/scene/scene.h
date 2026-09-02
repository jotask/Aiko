#pragma once

#include <aiko_types.h>
#include <typeindex>
#include <unordered_map>

#include "models/game_object.h"

namespace aiko
{

    class Scene
    {
        friend class GameObject;
    public:
        Scene();
        ~Scene();

        GameObject* create(string name);
        GameObject* create(GameObject* parent, string name);

        bool remove(const GameObject* obj);

        void clear();

        vector<GameObject*> getObjects();
        vector<const GameObject*> getObjects() const;

        void setActiveCamera(GameObject* obj);
        GameObject* getActiveCamera() { return m_activeCamera; }
        const GameObject* getActiveCamera() const { return m_activeCamera; }

        template<class T>
        vector<T*> components();

        template<class T>
        vector<const T*> components() const;

    private:
        using ComponentBucket = vector<Component*>;
        std::unordered_map<std::type_index, ComponentBucket> m_componentIndex;

        vector<AikoUPtr<GameObject>> m_objects;
        GameObject* m_activeCamera = nullptr;

        void destroyObject(GameObject& object);

        void registerComponent(Component* component, std::type_index type);
        void unregisterComponent(Component* component, std::type_index type);
        void registerObjectComponents(GameObject& object);
        void unregisterObjectComponents(GameObject& object);

    };

    template<class T>
    vector<T*> Scene::components()
    {
        vector<T*> result;
        if constexpr (std::is_base_of_v<Component, T>)
        {
            auto it = m_componentIndex.find(std::type_index(typeid(T)));
            if (it == m_componentIndex.end())
            {
                return result;
            }
            result.reserve(it->second.size());
            for (Component* component : it->second)
            {
                result.push_back(static_cast<T*>(component));
            }
        }
        else
        {
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
        }
        return result;
    }

    template<class T>
    vector<const T*> Scene::components() const
    {
        vector<const T*> result;
        if constexpr (std::is_base_of_v<Component, T>)
        {
            auto it = m_componentIndex.find(std::type_index(typeid(T)));
            if (it == m_componentIndex.end())
            {
                return result;
            }
            result.reserve(it->second.size());
            for (const Component* component : it->second)
            {
                result.push_back(static_cast<const T*>(component));
            }
        }
        else
        {
            for (const auto& object : m_objects)
            {
                if (object == nullptr)
                {
                    continue;
                }
                for (const T* component : object->getComponents<T>())
                {
                    result.push_back(component);
                }
            }
        }
        return result;
    }

}
