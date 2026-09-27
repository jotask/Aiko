#include "set_transform_command.h"

#include "models/game_object.h"
#include "systems/scene_system.h"

namespace aiko::editor
{

    SetTransformCommand::SetTransformCommand(SceneSystem& sceneSystem, GameObject& object, vec3 position, vec3 rotation, vec3 scale)
        : m_sceneSystem(&sceneSystem)
        , m_objectId(object.uuid())
        , m_oldState
            {
                .position = object.transform().position,
                .rotation = object.transform().rotation,
                .scale = object.transform().scale,
            }
        , m_newState
            {
                .position = position,
                .rotation = rotation,
                .scale = scale,
            }
    {
    }

    void SetTransformCommand::execute()
    {
        GameObject* object = m_sceneSystem->findGameObject(m_objectId);
        AIKO_ASSERT(object != nullptr, "SetTransformCommand object no longer exists");
        if (object == nullptr)
        {
            return;
        }
        Transform& transform = object->transform();
        transform.position = m_newState.position;
        transform.rotation = m_newState.rotation;
        transform.scale = m_newState.scale;
    }

    void SetTransformCommand::undo()
    {
        GameObject* object = m_sceneSystem->findGameObject(m_objectId);
        AIKO_ASSERT(object != nullptr, "SetTransformCommand object no longer exists");
        if (object == nullptr)
        {
            return;
        }
        Transform& transform = object->transform();
        transform.position = m_oldState.position;
        transform.rotation = m_oldState.rotation;
        transform.scale = m_oldState.scale;
    }

}
