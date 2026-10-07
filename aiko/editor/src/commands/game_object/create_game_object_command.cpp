#include "create_game_object_command.h"

#include <utility>

#include "models/game_object.h"
#include "systems/scene_system.h"

namespace aiko::editor
{

    CreateGameObjectCommand::CreateGameObjectCommand(SceneSystem& sceneSystem, GameObject* parent, string name)
        : m_sceneSystem(&sceneSystem)
        , m_name(std::move(name))
    {
        if (parent != nullptr)
        {
            m_parentId = parent->uuid();
        }
    }

    void CreateGameObjectCommand::execute()
    {
        AIKO_ASSERT(m_sceneSystem != nullptr, "CreateGameObjectCommand has no SceneSystem");

        if (m_sceneSystem == nullptr)
        {
            return;
        }

        GameObject* parent = nullptr;

        if (m_parentId.has_value())
        {
            parent = m_sceneSystem->findGameObject(*m_parentId);
            AIKO_ASSERT(parent != nullptr, "CreateGameObjectCommand parent no longer exists");

            if (parent == nullptr)
            {
                return;
            }
        }

        m_sceneSystem->createGameObject(m_objectId, parent, m_name);
    }

    void CreateGameObjectCommand::undo()
    {
        AIKO_ASSERT(m_sceneSystem != nullptr, "CreateGameObjectCommand has no SceneSystem");

        if (m_sceneSystem == nullptr)
        {
            return;
        }

        GameObject* object = m_sceneSystem->findGameObject(m_objectId);
        AIKO_ASSERT(object != nullptr, "CreateGameObjectCommand object no longer exists");

        if (object == nullptr)
        {
            return;
        }

        m_sceneSystem->destroyGameObject(object);
    }

    GameObject* CreateGameObjectCommand::createdObject() const
    {
        if (m_sceneSystem == nullptr)
        {
            return nullptr;
        }
        return m_sceneSystem->findGameObject(m_objectId);
    }

}
