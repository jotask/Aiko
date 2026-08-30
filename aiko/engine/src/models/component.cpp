#include "component.h"

#include "game_object.h"

namespace aiko
{

    void Component::markAssetBindingDirty()
    {
        if (gameobject != nullptr)
        {
            gameobject->markAssetBindingDirty();
        }
    }

    void Component::setup(GameObject* obj)
    {
        gameobject = obj;
    }

}
