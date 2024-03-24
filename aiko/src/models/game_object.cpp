#include "game_object.h"

#include <stdexcept>

namespace aiko
{

    aiko::AikoPtr<Transform> GameObject::transform()
    {
        return getComponent<Transform>();
    }

    void GameObject::update()
    {

    }

    void GameObject::render()
    {

    }

}
