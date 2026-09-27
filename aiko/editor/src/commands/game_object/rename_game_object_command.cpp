#include "rename_game_object_command.h"

#include "models/game_object.h"
#include "systems/scene_system.h"

#include <utility>

namespace aiko::editor
{

    RenameGameObjectCommand::RenameGameObjectCommand(SceneSystem& sceneSystem, GameObject& object, string newName)
        : m_sceneSystem(&sceneSystem)
       , m_objectId(object.uuid())
       , m_oldName(object.getName())
       , m_newName(std::move(newName))
    {
    }

    void RenameGameObjectCommand::execute()
    {
        GameObject* object = m_sceneSystem->findGameObject(m_objectId);
        AIKO_ASSERT(object != nullptr, "RenameGameObjectCommand object no longer exists");
        if (object == nullptr)
        {
            return;
        }
        object->setName(m_newName);
    }

    void RenameGameObjectCommand::undo()
    {
        GameObject* object = m_sceneSystem->findGameObject(m_objectId);
        AIKO_ASSERT(object != nullptr, "RenameGameObjectCommand object no longer exists");
        if (object == nullptr)
        {
            return;
        }
        object->setName(m_oldName);
    }

}
