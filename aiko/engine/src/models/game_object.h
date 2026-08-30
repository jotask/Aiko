#pragma once

#include <functional>
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
    public:

        GameObject() = default;
        ~GameObject() = default;

        template<class T, typename... Args>
        AikoPtr<T> addComponent(Args...);

        template<class T>
        bool hasComponent();

        template<class T>
        AikoPtr<T> getComponent();

        template<class T>
        bool removeComponent();

        template<class T>
        bool removeComponent(T*);

        string getName() const { return name; }
        void setName( string newName ) { name = newName; }

        Transform& transform();

        vector<Component*> getComponents();

        const uuid::Uuid& uuid() const { return m_uuid; }

    private:

        EntityId m_entity;
        uuid::Uuid m_uuid;

        string name;
        vector<AikoPtr<Component>> m_components;

        void update();
        void render();
        void dispose();

    };

    template<class T, typename... Args>
    AikoPtr<T> GameObject::addComponent(Args... args)
    {
        if (hasComponent<T>() == true)
        {
            logger::Log::error("Couldn't add Component");
            return  nullptr;
        }
        m_components.emplace_back(std::make_shared<T>(args...));
        AikoPtr<Component> back = m_components.back();
        back->setup(this);
        back->init();
        return std::dynamic_pointer_cast<T>(back);
    }

    template<class T>
    bool GameObject::hasComponent()
    {
        auto it = std::find_if(m_components.begin(), m_components.end(), [](const aiko::AikoPtr<Component>& component) {
            return dynamic_cast<T*>(component.get()) != nullptr;
        });
        return it != m_components.end();
    }

    template<class T>
    AikoPtr<T> GameObject::getComponent()
    {
        auto it = std::find_if(m_components.begin(), m_components.end(), [](const std::shared_ptr<Component>& component) {
            return dynamic_cast<T*>(component.get()) != nullptr;
        });
        return (it != m_components.end()) ? std::dynamic_pointer_cast<T>(*it) : nullptr;
    }

    template<class T>
    bool GameObject::removeComponent()
    {
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

}
