#pragma once

#include <string>

#include "aiko_types.h"
#include "core/uuid.h"
#include "components/component_entity.h"

namespace aiko
{
    
    class GameObject;
    
    // TODO extract this from the base class
    class Component: public IUpdate, public IRender
    {
        friend class GameObject;
        friend class EntityComponentSystem;
        friend class ComponentRenderer;
    public:
        Component(std::string name)
            : m_name(name)
            , gameobject(nullptr)
        {
        };
    
        const uuid::Uuid uuid;
    
        virtual ~Component() = default;
    protected:
        GameObject* gameobject;
        virtual void init() { }
    private:
        const std::string m_name;
        void setup(GameObject* obj);
    };

}
