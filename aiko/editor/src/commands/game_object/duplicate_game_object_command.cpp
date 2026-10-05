#include "duplicate_game_object_command.h"

#include "models/game_object.h"
#include "registry/components_functionality.h"
#include "systems/scene_system.h"

namespace aiko::editor
{

    DuplicateGameObjectCommand::DuplicateGameObjectCommand(SceneSystem& sceneSystem, const GameObject& source)
        : m_sceneSystem(&sceneSystem)
        , m_name(source.getName() + " Copy")
        , m_active(source.isActiveSelf())
    {
        if (const GameObject* parent = source.getParent())
        {
            m_parentId = parent->uuid();
        }

        for (const Component* component : source.getComponents())
        {
            YAML::Node node;

            const bool serialized = component::serializeComponent(*component, node);

            AIKO_ASSERT(serialized, "Component is not supported by duplicate serialization");

            if (serialized)
            {
                m_components.emplace_back(node);
            }
        }
    }

    void DuplicateGameObjectCommand::execute()
    {
        AIKO_ASSERT(m_sceneSystem != nullptr, "DuplicateGameObjectCommand has no SceneSystem");

        if (m_sceneSystem == nullptr)
        {
            return;
        }

        GameObject* parent = nullptr;

        if (m_parentId.has_value())
        {
            parent = m_sceneSystem->findGameObject(*m_parentId);

            AIKO_ASSERT(parent != nullptr, "DuplicateGameObjectCommand parent no longer exists");

            if (parent == nullptr)
            {
                return;
            }
        }

        GameObject* object = m_sceneSystem->createGameObject(m_duplicateId, parent, m_name);

        AIKO_ASSERT(object != nullptr, "Failed to create duplicated GameObject");

        if (object == nullptr)
        {
            return;
        }

        object->setActive(m_active);

        for (const YAML::Node& componentNode : m_components)
        {
            const bool deserialized = component::deserializeComponent(componentNode, *object);

            AIKO_ASSERT(deserialized, "Failed to deserialize duplicated component");
        }
    }

    void DuplicateGameObjectCommand::undo()
    {
        AIKO_ASSERT(m_sceneSystem != nullptr, "DuplicateGameObjectCommand has no SceneSystem");

        if (m_sceneSystem == nullptr)
        {
            return;
        }

        GameObject* object = m_sceneSystem->findGameObject(m_duplicateId);

        AIKO_ASSERT(object != nullptr, "Duplicated GameObject no longer exists");

        if (object != nullptr)
        {
            m_sceneSystem->destroyGameObject(object);
        }
    }

    GameObject* DuplicateGameObjectCommand::duplicatedObject() const
    {
        if (m_sceneSystem == nullptr)
        {
            return nullptr;
        }

        return m_sceneSystem->findGameObject(m_duplicateId);
    }

}
