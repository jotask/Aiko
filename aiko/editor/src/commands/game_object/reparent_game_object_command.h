#pragma once

#include <optional>

#include "commands/editor_command.h"
#include "core/uuid.h"

namespace aiko
{
    class GameObject;
    class SceneSystem;
}

namespace aiko::editor
{

    class ReparentGameObjectCommand final : public EditorCommand
    {
    public:
        ReparentGameObjectCommand(SceneSystem& sceneSystem, GameObject& object, GameObject* newParent);

        void execute() override;
        void undo() override;

    private:
        SceneSystem* m_sceneSystem = nullptr;

        uuid::Uuid m_objectId;

        std::optional<uuid::Uuid> m_oldParentId;
        std::optional<uuid::Uuid> m_newParentId;
    };

}
