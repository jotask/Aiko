#include "game_object.h"

#include "components/transform_component.h"
#include "scene/scene.h"

namespace aiko
{

    GameObject::GameObject()
    {
        addComponent<TransformComponent>();
    }

    Transform& GameObject::transform()
    {
        return getComponent<TransformComponent>()->transform;
    }

    const Transform& GameObject::transform() const
    {
        return getComponent<TransformComponent>()->transform;
    }

    bool GameObject::removeComponent(Component* item)
    {
        if (item == nullptr)
        {
            return false;
        }

        if (dynamic_cast<TransformComponent*>(item) != nullptr)
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
