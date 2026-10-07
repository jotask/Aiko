#include "component.h"

#include "models/game_object.h"

namespace aiko
{

    void Component::setup(GameObject* obj)
    {
        gameobject = obj;
    }

    bool Component::isActiveAndEnabled() const
    {
        if (m_enabled == false)
        {
            return false;
        }

        if (gameobject == nullptr)
        {
            return false;
        }

        return gameobject->isActiveInHierarchy();
    }

}