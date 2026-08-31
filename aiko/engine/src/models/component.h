#pragma once

#include "aiko_types.h"
#include "core/uuid.h"

namespace aiko
{
    
    class GameObject;
    
    class Component
    {
        friend class GameObject;
    public:

        Component(const Component&) = delete;
        Component& operator= (const Component&) = delete;

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

        void markAssetBindingDirty();

        virtual void init() { }
        virtual void dispose() { }
    private:
        const string m_name;
        void setup(GameObject* obj);
    };

}
