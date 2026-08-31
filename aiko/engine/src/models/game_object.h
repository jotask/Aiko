#pragma once

#include <functional>
#include <utility>
#include <type_traits>
#include <algorithm>

#include "core/uuid.h"
#include "aiko_types.h"
#include "component.h"
#include "components/transform_component.h"
#include "models/entity_id.h"

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
        EntityId m_entity;
        uuid::Uuid m_uuid;

        string name;
        vector<AikoUPtr<Component>> m_components;

        void dispose();

        template<class T>
        T* findComponent();

        template<class T>
        const T* findComponent() const;

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
        baseComponent->setup(this);
        baseComponent->init();
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
        for (size_t i = 0; i < m_components.size(); ++i)
        {
            if (auto* component = dynamic_cast<T*>(m_components[i].get()))
            {
                components.push_back(component);
            }
        }
        return components;
    }

    template<class T>
    vector<const T*> GameObject::getComponents() const
    {
        vector<const T*> components;
        for (size_t i = 0; i < m_components.size(); ++i)
        {
            if (auto* component = dynamic_cast<const T*>(m_components[i].get()))
            {
                components.push_back(component);
            }
        }
        return components;
    }

    template<class T>
    size_t GameObject::removeComponents()
    {
        if constexpr (std::is_same_v<T, TransformComponent>)
        {
            return 0;
        }
        size_t removedCount = 0;

        for (auto it = m_components.begin(); it != m_components.end();)
        {
            if (dynamic_cast<T*>(it->get()) != nullptr)
            {
                (*it)->dispose();
                it = m_components.erase(it);
                ++removedCount;
            }
            else
            {
                ++it;
            }
        }

        return removedCount;
    }

    template<class T>
    T* GameObject::findComponent()
    {
        auto it = std::find_if(m_components.begin(), m_components.end(), [](const AikoUPtr<Component>& component)
        {
            return dynamic_cast<T*>(component.get()) != nullptr;
        });
        return (it != m_components.end()) ? dynamic_cast<T*>(it->get()) : nullptr;
    }

    template<class T>
    const T* GameObject::findComponent() const
    {
        auto it = std::find_if(m_components.begin(), m_components.end(), [](const AikoUPtr<Component>& component)
        {
            return dynamic_cast<const T*>(component.get()) != nullptr;
        });
        return (it != m_components.end()) ? dynamic_cast<const T*>(it->get()) : nullptr;
    }

}
