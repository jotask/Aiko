#include "transform_command.h"

#include <utility>

#include "models/game_object.h"
#include "systems/scene_system.h"

namespace aiko::editor
{

    TransformCommand::TransformCommand(SceneSystem& sceneSystem, uuid::Uuid objectId, TransformState before, TransformState after)
        : m_sceneSystem(&sceneSystem)
        , m_objectId(std::move(objectId))
        , m_before(before)
        , m_after(after)
    {
    }

    void TransformCommand::execute()
    {
        apply(m_after);
    }

    void TransformCommand::undo()
    {
        apply(m_before);
    }

    void TransformCommand::apply(const TransformState& state)
    {
        GameObject* object = m_sceneSystem->findGameObject(m_objectId);

        if (object == nullptr)
        {
            return;
        }

        Transform& transform = object->transform();

        transform.position = state.position;
        transform.rotation = state.rotation;
        transform.scale = state.scale;
    }

}
