#pragma once



#include "aiko_types.h"
#include "core/uuid.h"
#include "components/component_entity.h"

namespace aiko
{
    
    class GameObject;
    
    class Component
    {
        friend class GameObject;
        friend class EntityComponentSystem;
        friend class ComponentRenderer;
    public:
        Component(string name)
            : m_name(name)
            , gameobject(nullptr)
        {
        };
    
        const uuid::Uuid uuid;
    
        virtual ~Component() = default;
        const char* getName() const { return m_name.c_str(); };
    protected:
        GameObject* gameobject;
        virtual void init() { }
    private:
        const string m_name;
        void setup(GameObject* obj);
    };

}
