#pragma once

#include "aiko_types.h"
#include "core/uuid.h"
#include "component_context.h"

namespace aiko
{
    
    class GameObject;
    
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
        virtual void init() { }
        virtual void dispose() { }
    private:
        const string m_name;
        ComponentContext* m_context;
        void setup(GameObject* obj, ComponentContext* context);
    };

}
