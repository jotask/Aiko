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
        for (auto& go : m_components)
        {
            Component* c = go.get();
            if (IUpdate* updateComponent = dynamic_cast<IUpdate*>(c))
            {
                updateComponent->update();
            }
        }
    }

    void GameObject::render()
    {
        for (auto& go : m_components)
        {
            Component* c = go.get();
            if (IRender* renderComponent = dynamic_cast<IRender*>(c))
            {
                renderComponent->render();
            }
        }
    }

}
