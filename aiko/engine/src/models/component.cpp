#include "component.h"

namespace aiko
{
    
    void Component::setup(GameObject* obj, ComponentContext* context, EntityRegistry* entityRegistry, EntityId entity)
    {
        gameobject = obj;
        m_context = context;
        m_entityRegistry = entityRegistry;
        m_entity = entity;
    }

}
