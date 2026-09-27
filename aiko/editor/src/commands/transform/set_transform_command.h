#pragma once

#include "commands/editor_command.h"
#include "core/uuid.h"
#include "math/math_vector.h"

#include <aiko_types.h>

namespace aiko
{
    class GameObject;
    class SceneSystem;
}

namespace aiko::editor
{

    class SetTransformCommand final : public EditorCommand
    {
    public:
        SetTransformCommand(SceneSystem& sceneSystem, GameObject& object, vec3 position, vec3 rotation, vec3 scale);

        void execute() override;
        void undo() override;

    private:
        struct State
        {
            vec3 position;
            vec3 rotation;
            vec3 scale;
        };

        SceneSystem* m_sceneSystem = nullptr;
        uuid::Uuid m_objectId;

        State m_oldState;
        State m_newState;
    };

}
