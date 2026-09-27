#include "delete_game_object_command.h"

#include "models/game_object.h"
#include "registry/components_functionality.h"
#include "systems/scene_system.h"

#include <utility>

namespace aiko::editor
{

    DeleteGameObjectCommand::DeleteGameObjectCommand(SceneSystem& sceneSystem, GameObject& object)
        : m_sceneSystem(&sceneSystem)
        , m_objectId(object.uuid())
        , m_name(object.getName())
        , m_active(object.isActiveSelf())
    {
        if (GameObject* parent = object.getParent())
        {
            m_parentId = parent->uuid();
        }
        for (const Component* component : object.getComponents())
        {
            YAML::Node node;
            if (component::serializeComponent(*component, node))
            {
                m_components.emplace_back(node);
            }
        }
        for (GameObject* child : object.getChildren())
        {
            if (child != nullptr)
            {
                m_childIds.emplace_back(child->uuid());
            }
        }
    }

    void DeleteGameObjectCommand::execute()
    {
        AIKO_ASSERT(m_sceneSystem != nullptr, "DeleteGameObjectCommand has no SceneSystem");

        if (m_sceneSystem == nullptr)
        {
            return;
        }

        GameObject* object = m_sceneSystem->findGameObject(m_objectId);

        AIKO_ASSERT(object != nullptr, "DeleteGameObjectCommand object no longer exists");

        if (object == nullptr)
        {
            return;
        }

        m_sceneSystem->destroyGameObject(object);
    }

    void DeleteGameObjectCommand::undo()
    {
        AIKO_ASSERT(m_sceneSystem != nullptr, "DeleteGameObjectCommand has no SceneSystem");

        if (m_sceneSystem == nullptr)
        {
            return;
        }

        GameObject* parent = nullptr;

        if (m_parentId.has_value())
        {
            parent = m_sceneSystem->findGameObject(*m_parentId);

            AIKO_ASSERT(parent != nullptr, "DeleteGameObjectCommand parent no longer exists");

            if (parent == nullptr)
            {
                return;
            }
        }

        GameObject* object = m_sceneSystem->createGameObject(m_objectId, parent, m_name);

        object->setActive(m_active);

        for (const YAML::Node& componentNode : m_components)
        {
            component::deserializeComponent(componentNode, *object);
        }

        for (const uuid::Uuid& childId : m_childIds)
        {
            GameObject* child = m_sceneSystem->findGameObject(childId);
            if (child != nullptr)
            {
                child->transform().setParent(&object->transform());
            }
        }

    }

}
