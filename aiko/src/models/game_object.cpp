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

    std::vector<Component*> GameObject::getComponents()
    {
        std::vector<Component*> components;
        for (auto& go : m_components)
        {
            components.push_back(go.get());
        }
        return components;
    }

    void GameObject::render()
    {
        for (auto& go : m_components)
        {
            if (IRender* renderComponent = dynamic_cast<IRender*>(go.get()))
            {
                renderComponent->render();
            }
        }
    }

}
