#include "component.h"

namespace aiko
{
    
    void Component::setup(GameObject* obj, ComponentContext* context)
    {
        this->gameobject = obj;
        this->m_context = context;
    }

}
