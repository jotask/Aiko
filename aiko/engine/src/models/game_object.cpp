#include "game_object.h"

#include <utility>

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

    GameObject* GameObject::getParent()
    {
        Transform* parent = transform().getParent();

        if (parent == nullptr || m_scene == nullptr)
        {
            return nullptr;
        }

        for (GameObject* object : m_scene->getObjects())
        {
            if (&object->transform() == parent)
            {
                return object;
            }
        }

        return nullptr;
    }

    const GameObject* GameObject::getParent() const
    {
        const Transform* parent = transform().getParent();

        if (parent == nullptr || m_scene == nullptr)
        {
            return nullptr;
        }

        for (const GameObject* object : std::as_const(*m_scene).getObjects())
        {
            if (&object->transform() == parent)
            {
                return object;
            }
        }

        return nullptr;
    }

    vector<GameObject*> GameObject::getChildren()
    {
        vector<GameObject*> result;

        if (m_scene == nullptr)
        {
            return result;
        }

        const vector<Transform*>& children = transform().getChildren();
        result.reserve(children.size());

        for (Transform* child : children)
        {
            for (GameObject* object : m_scene->getObjects())
            {
                if (&object->transform() == child)
                {
                    result.push_back(object);
                    break;
                }
            }
        }

        return result;
    }

    vector<const GameObject*> GameObject::getChildren() const
    {
        vector<const GameObject*> result;

        if (m_scene == nullptr)
        {
            return result;
        }

        const vector<Transform*>& children = transform().getChildren();
        result.reserve(children.size());

        for (const Transform* child : children)
        {
            for (const GameObject* object : std::as_const(*m_scene).getObjects())
            {
                if (&object->transform() == child)
                {
                    result.push_back(object);
                    break;
                }
            }
        }

        return result;
    }

    vector<Component*> GameObject::getComponents()
    {
        vector<Component*> result;
        result.reserve(m_components.size());

        for (const auto& component : m_components)
        {
            result.push_back(component.get());
        }

        return result;
    }

    vector<const Component*> GameObject::getComponents() const
    {
        vector<const Component*> result;
        result.reserve(m_components.size());

        for (const auto& component : m_components)
        {
            result.push_back(component.get());
        }

        return result;
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

        notifyComponentRemoving(item, type);

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

    void GameObject::notifyComponentAdded(Component* component, std::type_index type)
    {
        if (m_scene != nullptr)
        {
            m_scene->registerComponent(component, type);
        }
    }

    void GameObject::notifyComponentRemoving(Component* component, std::type_index type)
    {
        if (m_scene != nullptr)
        {
            m_scene->unregisterComponent(component, type);
        }
    }

}
