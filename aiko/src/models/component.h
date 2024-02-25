#pragma once

#include <string>

#include "aiko_types.h"
#include "core/uuid.h"

namespace aiko
{
    
    class GameObject;
    
    class Component
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
        virtual void update() { };
        virtual void render() { };
    private:
        const std::string m_name;
        void setup(GameObject* obj);
    };

}
