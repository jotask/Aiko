#pragma once

#include <functional>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <typeindex>
#include <unordered_map>

#include "core/uuid.h"
#include "aiko_types.h"
#include "component.h"
#include "components/transform_component.h"

namespace aiko
{

    class Scene;

    class GameObject
    {
        friend class Scene;
    public:

        GameObject();
        ~GameObject() = default;

        template<class T, typename... Args>
        T* addComponent(Args&&...);

        template<class T>
        bool hasComponent() const;

        template<class T>
        T* getComponent();

        template<class T>
        const T* getComponent() const;

        template<class T>
        vector<T*> getComponents();

        template<class T>
        vector<const T*> getComponents() const;

        template<class T>
        size_t removeComponents();

        bool removeComponent(Component*);

        string getName() const { return name; }
        void setName( string newName ) { name = newName; }

        Transform& transform();
        const Transform& transform() const;

        const uuid::Uuid& uuid() const { return m_uuid; }

    private:

        Scene* m_scene = nullptr;
        uuid::Uuid m_uuid;

        string name;

        using ComponentBucket = vector<Component*>;

        vector<AikoUPtr<Component>> m_components;
        std::unordered_map<std::type_index, ComponentBucket> m_componentIndex;

        void dispose();

        template<class T>
        T* findComponent();

        template<class T>
        const T* findComponent() const;

        void notifyComponentAdded(Component*, std::type_index);
        void notifyComponentRemoving(Component*, std::type_index);

    };

    template<class T, typename... Args>
    T* GameObject::addComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<Component, T>, "GameObject::addComponent requires a Component type");
        if constexpr (std::is_same_v<T, TransformComponent>)
        {
            if (hasComponent<TransformComponent>())
            {
                logger::Log::error("Couldn't add another TransformComponent");
                return nullptr;
            }
        }
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* result = component.get();
        Component* baseComponent = component.get();
        m_components.emplace_back(std::move(component));
        m_componentIndex[std::type_index(typeid(T))].push_back(baseComponent);
        baseComponent->setup(this);
        baseComponent->init();
        notifyComponentAdded(baseComponent, std::type_index(typeid(T)));
        return result;
    }

    template<class T>
    bool GameObject::hasComponent() const
    {
        return findComponent<T>() != nullptr;
    }

    template<class T>
    T* GameObject::getComponent()
    {
        return findComponent<T>();
    }

    template<class T>
    const T* GameObject::getComponent() const
    {
        return findComponent<T>();
    }

    template<class T>
    vector<T*> GameObject::getComponents()
    {
        vector<T*> components;
        if constexpr (std::is_base_of_v<Component, T>)
        {
            auto it = m_componentIndex.find(std::type_index(typeid(T)));
            if (it == m_componentIndex.end())
            {
                return components;
            }
            components.reserve(it->second.size());
            for (Component* component : it->second)
            {
                components.push_back(static_cast<T*>(component));
            }
        }
        else
        {
            for (const auto& component : m_components)
            {
                if (auto* result = dynamic_cast<T*>(component.get()))
                {
                    components.push_back(result);
                }
            }
        }
        return components;
    }

    template<class T>
    vector<const T*> GameObject::getComponents() const
    {
        vector<const T*> components;
        if constexpr (std::is_base_of_v<Component, T>)
        {
            auto it = m_componentIndex.find(std::type_index(typeid(T)));
            if (it == m_componentIndex.end())
            {
                return components;
            }
            components.reserve(it->second.size());
            for (const Component* component : it->second)
            {
                components.push_back(static_cast<const T*>(component));
            }
        }
        else
        {
            for (const auto& component : m_components)
            {
                if (auto* result = dynamic_cast<const T*>(component.get()))
                {
                    components.push_back(result);
                }
            }
        }
        return components;
    }

    template<class T>
    size_t GameObject::removeComponents()
    {
        static_assert(std::is_base_of_v<Component, T>, "GameObject::removeComponents requires a Component type");
        if constexpr (std::is_same_v<T, TransformComponent>)
        {
            return 0;
        }
        const std::type_index type = std::type_index(typeid(T));
        auto bucket = m_componentIndex.find(type);
        if (bucket == m_componentIndex.end())
        {
            return 0;
        }
        const size_t removedCount = bucket->second.size();
        for (auto it = m_components.begin(); it != m_components.end();)
        {
            if (typeid(*(*it)) == typeid(T))
            {
                notifyComponentRemoving(it->get(), type);
                (*it)->dispose();
                it = m_components.erase(it);
            }
            else
            {
                ++it;
            }
        }
        m_componentIndex.erase(bucket);
        return removedCount;
    }

    template<class T>
    T* GameObject::findComponent()
    {
        static_assert(std::is_base_of_v<Component, T>, "GameObject::findComponent requires a Component type");
        auto it = m_componentIndex.find(std::type_index(typeid(T)));
        if (it == m_componentIndex.end() || it->second.empty())
        {
            return nullptr;
        }
        return static_cast<T*>(it->second.front());
    }

    template<class T>
    const T* GameObject::findComponent() const
    {
        static_assert(std::is_base_of_v<Component, T>, "GameObject::findComponent requires a Component type");
        auto it = m_componentIndex.find(std::type_index(typeid(T)));
        if (it == m_componentIndex.end() || it->second.empty())
        {
            return nullptr;
        }
        return static_cast<const T*>(it->second.front());
    }

}
