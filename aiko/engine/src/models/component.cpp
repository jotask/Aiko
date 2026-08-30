#include "component.h"

namespace aiko
{
    
    void Component::setup(GameObject* obj, ComponentContext* context)
    {
        gameobject = obj;
        m_context = context;
    }

}
