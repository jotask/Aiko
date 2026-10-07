#include "reparent_game_object_command.h"

#include "models/game_object.h"
#include "systems/scene_system.h"

namespace aiko::editor
{

    ReparentGameObjectCommand::ReparentGameObjectCommand(SceneSystem& sceneSystem, GameObject& object, GameObject* newParent)
        : m_sceneSystem(&sceneSystem)
        , m_objectId(object.uuid())
    {
        if (GameObject* oldParent = object.getParent())
        {
            m_oldParentId = oldParent->uuid();
        }

        if (newParent != nullptr)
        {
            m_newParentId = newParent->uuid();
        }
    }

    void ReparentGameObjectCommand::execute()
    {
        AIKO_ASSERT(m_sceneSystem != nullptr, "ReparentGameObjectCommand has no SceneSystem");

        if (m_sceneSystem == nullptr)
        {
            return;
        }

        GameObject* object = m_sceneSystem->findGameObject(m_objectId);

        AIKO_ASSERT(object != nullptr, "ReparentGameObjectCommand object no longer exists");

        if (object == nullptr)
        {
            return;
        }

        if (m_newParentId.has_value())
        {
            GameObject* parent = m_sceneSystem->findGameObject(*m_newParentId);

            AIKO_ASSERT(parent != nullptr, "ReparentGameObjectCommand new parent no longer exists");

            if (parent == nullptr)
            {
                return;
            }

            object->transform().setParent(&parent->transform());
        }
        else
        {
            object->transform().setParent(nullptr);
        }
    }

    void ReparentGameObjectCommand::undo()
    {
        AIKO_ASSERT(m_sceneSystem != nullptr, "ReparentGameObjectCommand has no SceneSystem");

        if (m_sceneSystem == nullptr)
        {
            return;
        }

        GameObject* object = m_sceneSystem->findGameObject(m_objectId);

        AIKO_ASSERT(object != nullptr, "ReparentGameObjectCommand object no longer exists");

        if (object == nullptr)
        {
            return;
        }

        if (m_oldParentId.has_value())
        {
            GameObject* parent = m_sceneSystem->findGameObject(*m_oldParentId);

            AIKO_ASSERT(parent != nullptr, "ReparentGameObjectCommand old parent no longer exists");

            if (parent == nullptr)
            {
                return;
            }

            object->transform().setParent(&parent->transform());
        }
        else
        {
            object->transform().setParent(nullptr);
        }
    }

}
