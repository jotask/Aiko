#pragma once

#include "commands/editor_command.h"
#include "core/uuid.h"
#include "math/math_vector.h"
#include "transform_state.h"

#include <aiko_types.h>

namespace aiko
{
    class SceneSystem;
}

namespace aiko::editor
{

    class TransformCommand final : public EditorCommand
    {
    public:
        TransformCommand(SceneSystem& sceneSystem, uuid::Uuid objectId, TransformState before, TransformState after);

        void execute() override;
        void undo() override;

    private:
        void apply(const TransformState& state);

        SceneSystem* m_sceneSystem = nullptr;
        uuid::Uuid m_objectId;

        TransformState m_before;
        TransformState m_after;
    };

}
