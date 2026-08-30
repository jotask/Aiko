#pragma once

#include "aiko_types.h"
#include "core/uuid.h"
#include "component_context.h"
#include "models/entity_id.h"

namespace aiko
{
    
    class GameObject;
    class EntityRegistry;
    
    class Component
    {
        friend class GameObject;
        friend class EntityComponentSystem;
        friend class ComponentRenderer;
    public:

        Component(const Component&) = delete;
        Component& operator= (const Component&) = delete;

        Component(string name)
            : m_name(name)
            , gameobject(nullptr)
            , m_context(nullptr)
        {
        };
    
        const uuid::Uuid uuid;
    
        virtual ~Component() = default;
        const char* getName() const { return m_name.c_str(); };
    protected:
        GameObject* gameobject;
        ComponentContext& context()
        {
            AIKO_ASSERT(m_context != nullptr, "Component context not initialized");
            return *m_context;
        }

        const ComponentContext& context() const
        {
            AIKO_ASSERT(m_context != nullptr, "Component context not initialized");
            return *m_context;
        }
        EntityRegistry& entityRegistry()
        {
            AIKO_ASSERT(m_entityRegistry != nullptr, "Component entity registry not initialized");
            return *m_entityRegistry;
        }

        const EntityRegistry& entityRegistry() const
        {
            AIKO_ASSERT(m_entityRegistry != nullptr, "Component entity registry not initialized");
            return *m_entityRegistry;
        }

        EntityId entity() const
        {
            return m_entity;
        }
        virtual void init() { }
        virtual void dispose() { }
    private:
        const string m_name;
        EntityRegistry* m_entityRegistry = nullptr;
        EntityId m_entity;
        ComponentContext* m_context;
        void setup(GameObject* obj, ComponentContext* context, EntityRegistry* entityRegistry, EntityId entity);
    };

}
