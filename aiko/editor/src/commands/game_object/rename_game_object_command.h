#pragma once

#include "commands/editor_command.h"
#include "core/uuid.h"

#include <aiko_types.h>

namespace aiko
{
    class GameObject;
    class SceneSystem;
}

namespace aiko::editor
{

    class RenameGameObjectCommand final : public EditorCommand
    {
    public:
        RenameGameObjectCommand(SceneSystem& sceneSystem, GameObject& object, string newName);

        void execute() override;
        void undo() override;

    private:
        SceneSystem* m_sceneSystem = nullptr;
        uuid::Uuid m_objectId;

        string m_oldName;
        string m_newName;
    };

}
