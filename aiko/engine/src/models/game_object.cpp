#include "game_object.h"

#include "components/transform_component.h"

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

        const std::type_index type = std::type_index(typeid(*item));

        auto bucketIt = m_componentIndex.find(type);
        if (bucketIt != m_componentIndex.end())
        {
            auto& bucket = bucketIt->second;

            std::erase(bucket, item);

            if (bucket.empty())
            {
                m_componentIndex.erase(bucketIt);
            }
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
        m_componentIndex.clear();
    }

}
