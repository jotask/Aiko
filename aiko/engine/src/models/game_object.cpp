#include "game_object.h"

#include "components/component_entity.h"
#include "components/transform_component.h"
#include "scene/scene.h"

namespace aiko
{

    Transform& GameObject::transform()
    {
        return getComponent<TransforComponent>()->transform;
    }

    const Transform& GameObject::transform() const
    {
        return getComponent<TransforComponent>()->transform;
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

    vector<Component*> GameObject::getComponents()
    {
        vector<Component*> components;
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

    void GameObject::dispose()
    {
        for (const auto& component : m_components)
        {
            if (component != nullptr)
            {
                component->dispose();
            }
        }
        m_components.clear();
    }

    void GameObject::markAssetBindingDirty()
    {
        if (m_scene != nullptr)
        {
            m_scene->markAssetBindingDirty(*this);
        }
    }

}
