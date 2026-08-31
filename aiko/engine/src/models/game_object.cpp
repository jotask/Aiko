#include "game_object.h"

#include "components/component_entity.h"
#include "components/transform_component.h"
#include "scene/scene.h"

namespace aiko
{

    GameObject::GameObject()
    {
        addComponent<TransforComponent>();
    }

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

    bool GameObject::removeComponent(Component* item)
    {
        if (item == nullptr)
        {
            return false;
        }

        if (dynamic_cast<TransforComponent*>(item) != nullptr)
        {
            return false;
        }
        auto it = std::find_if(m_components.begin(), m_components.end(), [item](const AikoUPtr<Component>& component)
            {
                return component.get() == item;
            });

        if (it == m_components.end())
        {
            return false;
        }
        (*it)->dispose();
        m_components.erase(it);
        return true;
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
