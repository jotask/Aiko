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
        for (auto& cmp : m_components) cmp->update();
    }

    void GameObject::render()
    {
        for (auto& cmp : m_components) cmp->render();
    }

}
