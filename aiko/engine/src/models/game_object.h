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

    class SceneSystem;
    class Scene;

    class GameObject
    {
        friend class SceneSystem;
        friend class Scene;
        friend class Component;
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
        bool removeComponent();

        template<class T>
        bool removeComponent(T*);

        string getName() const { return name; }
        void setName( string newName ) { name = newName; }

        Transform& transform();
        const Transform& transform() const;

        vector<Component*> getComponents();

        const uuid::Uuid& uuid() const { return m_uuid; }

    private:

        Scene* m_scene = nullptr;
        EntityId m_entity;
        uuid::Uuid m_uuid;

        string name;
        vector<AikoUPtr<Component>> m_components;

        void update();
        void render();
        void dispose();

        template<class T>
        T* findComponent();

        template<class T>
        const T* findComponent() const;

        void markAssetBindingDirty();

    };

    template<class T, typename... Args>
    T* GameObject::addComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<Component, T>, "GameObject::addComponent requires a Component type");
        if (hasComponent<T>() == true)
        {
            logger::Log::error("Couldn't add Component");
            return  nullptr;
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
    bool GameObject::removeComponent()
    {
        if constexpr (std::is_same_v<T, TransforComponent>)
        {
            return false;
        }
        for (size_t i = 0; i < m_components.size(); ++i)
        {
            if (dynamic_cast<T*>(m_components[i].get()) != nullptr)
            {
                m_components[i]->dispose();
                m_components.erase(m_components.begin() + i);
                return true;
            }
        }
        return false;
    }

    template<class T>
    bool GameObject::removeComponent(T* item)
    {
        if (dynamic_cast<TransforComponent*>(item) != nullptr)
        {
            return false;
        }
        for (size_t i = 0; i < m_components.size(); ++i)
        {
            if (m_components[i].get() == item)
            {
                m_components[i]->dispose();
                m_components.erase(m_components.begin() + i);
                return true;
            }
        }
        return false;
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
