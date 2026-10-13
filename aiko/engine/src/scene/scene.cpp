#include "scene.h"

#include "models/game_object.h"
#include "components/camera_component.h"

#include <utility>
#include <algorithm>

namespace aiko
{

    Scene::Scene() = default;

    Scene::~Scene()
    {
        clear();
    }

    GameObject* Scene::create(string name)
    {
        return create(uuid::Uuid{}, std::move(name));
    }

    GameObject* Scene::create(const uuid::Uuid& id, string name)
    {
        AIKO_ASSERT(find(id) == nullptr, "Scene already contains a GameObject with this UUID");
        auto object = std::make_unique<GameObject>();
        object->m_uuid = id;
        object->setName(std::move(name));
        GameObject* result = object.get();
        result->m_scene = this;
        m_objects.emplace_back(std::move(object));
        m_objectIndex.emplace(result->uuid(), result);
        m_transformOwners.emplace(&result->transform(), result);
        registerObjectComponents(*result);
        return result;
    }

    GameObject* Scene::create(GameObject* parent, string name)
    {
        return create(uuid::Uuid{}, parent, std::move(name));
    }

    GameObject* Scene::create(const uuid::Uuid& id, GameObject* parent, string name)
    {
        GameObject* object = create(id, std::move(name));
        if (parent != nullptr)
        {
            AIKO_ASSERT(parent->m_scene == this, "Parent GameObject belongs to another Scene");
            if (parent->m_scene == this)
            {
                object->transform().setParent(&parent->transform());
            }
        }
        return object;
    }

    GameObject* Scene::find(const uuid::Uuid& id)
    {
        const auto it = m_objectIndex.find(id);
        return it != m_objectIndex.end() ? it->second : nullptr;
    }

    const GameObject* Scene::find(const uuid::Uuid& id) const
    {
        const auto it = m_objectIndex.find(id);
        return it != m_objectIndex.end() ? it->second : nullptr;
    }

    GameObject* Scene::findByTransform(const Transform* transform)
    {
        if (transform == nullptr)
        {
            return nullptr;
        }
        const auto it = m_transformOwners.find(transform);
        return it != m_transformOwners.end() ? it->second : nullptr;
    }

    const GameObject* Scene::findByTransform(const Transform* transform) const
    {
        if (transform == nullptr)
        {
            return nullptr;
        }
        const auto it = m_transformOwners.find(transform);
        return it != m_transformOwners.end() ? it->second : nullptr;
    }

    bool Scene::remove(const GameObject* obj)
    {
        if (obj == nullptr)
        {
            return false;
        }

        auto it = std::find_if(m_objects.begin(), m_objects.end(), [obj](const AikoUPtr<GameObject>& go)
        {
            return go != nullptr && go.get() == obj;
        });

        if (it == m_objects.end())
        {
            return false;
        }

        AikoUPtr<GameObject>& object = *it;

        if (m_activeCamera == object.get())
        {
            m_activeCamera = nullptr;
        }

        m_objectIndex.erase(object->uuid());
        m_transformOwners.erase(&object->transform());

        destroyObject(*object);

        m_objects.erase(it);

        return true;
    }

    void Scene::swap(Scene& other)
    {
        if (this == &other)
        {
            return;
        }

        m_objects.swap(other.m_objects);
        m_componentIndex.swap(other.m_componentIndex);
        m_objectIndex.swap(other.m_objectIndex);
        m_transformOwners.swap(other.m_transformOwners);

        std::swap(m_activeCamera, other.m_activeCamera);
        std::swap(m_clearColor, other.m_clearColor);
        std::swap(m_ambientLight, other.m_ambientLight);

        for (const auto& object : m_objects)
        {
            object->m_scene = this;
        }

        for (const auto& object : other.m_objects)
        {
            object->m_scene = &other;
        }
    }

    void Scene::clear()
    {
        for (const auto& object : m_objects)
        {
            if (object != nullptr)
            {
                destroyObject(*object);
            }
        }
        m_objects.clear();
        m_componentIndex.clear();
        m_objectIndex.clear();
        m_transformOwners.clear();
        m_activeCamera = nullptr;
    }

    vector<GameObject*> Scene::getObjects()
    {
        vector<GameObject*> objs;
        objs.reserve(m_objects.size());
        for (const auto& obj: m_objects)
        {
            objs.push_back(obj.get());
        }
        return objs;
    }

    vector<const GameObject*> Scene::getObjects() const
    {
        vector<const GameObject*> objs;
        objs.reserve(m_objects.size());
        for (const auto& obj: m_objects)
        {
            objs.push_back(obj.get());
        }
        return objs;
    }

    void Scene::setActiveCamera(GameObject* obj)
    {
        if (obj == nullptr)
        {
            m_activeCamera = nullptr;
            return;
        }

        const bool exists = std::any_of(m_objects.begin(), m_objects.end(),
            [obj](const AikoUPtr<GameObject>& object)
            {
                return object.get() == obj;
            });

        AIKO_ASSERT(exists, "Active camera GameObject does not belong to this Scene");
        if (exists == false)
        {
            return;
        }

        const bool hasCamera = obj->getComponent<CameraComponent>() != nullptr;
        AIKO_ASSERT(hasCamera, "Active camera GameObject has no CameraComponent");
        if (hasCamera == false)
        {
            return;
        }

        m_activeCamera = obj;
    }

    Camera* Scene::resolveCamera()
    {
        return const_cast<Camera*>(std::as_const(*this).resolveCamera());
    }

    const Camera* Scene::resolveCamera() const
    {
        if (const GameObject* active = m_activeCamera)
        {
            const CameraComponent* component = active->getComponent<CameraComponent>();
            if (component != nullptr && component->isActiveAndEnabled())
            {
                return &component->getCamera();
            }
        }

        for (const CameraComponent* component : components<CameraComponent>())
        {
            if (component != nullptr && component->isActiveAndEnabled())
            {
                return &component->getCamera();
            }
        }

        return nullptr;
    }

    void Scene::destroyObject(GameObject& object)
    {
        Transform& transform = object.transform();
        transform.clearChildren();
        transform.clearParent();
        unregisterObjectComponents(object);
        object.dispose();
        object.m_scene = nullptr;
    }

    void Scene::registerComponent(Component* component, std::type_index type)
    {
        if (component == nullptr)
        {
            return;
        }

        m_componentIndex[type].push_back(component);
    }

    void Scene::unregisterComponent(Component* component, std::type_index type)
    {
        if (component == nullptr)
        {
            return;
        }

        if (type == std::type_index(typeid(CameraComponent)) && component->getGameObject() == m_activeCamera)
        {
            m_activeCamera = nullptr;
        }

        auto it = m_componentIndex.find(type);
        if (it == m_componentIndex.end())
        {
            return;
        }

        auto& bucket = it->second;
        std::erase(bucket, component);

        if (bucket.empty())
        {
            m_componentIndex.erase(it);
        }
    }

    void Scene::registerObjectComponents(GameObject& object)
    {
        for (const auto& [type, components] : object.m_componentIndex)
        {
            for (Component* component : components)
            {
                registerComponent(component, type);
            }
        }
    }

    void Scene::unregisterObjectComponents(GameObject& object)
    {
        for (const auto& [type, components] : object.m_componentIndex)
        {
            for (Component* component : components)
            {
                unregisterComponent(component, type);
            }
        }
    }

}
