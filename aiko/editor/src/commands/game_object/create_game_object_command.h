#pragma once

#include <aiko_types.h>
#include <optional>
#include "core/uuid.h"

#include "commands/editor_command.h"

namespace aiko
{
    class GameObject;
    class SceneSystem;
}

namespace aiko::editor
{

    class CreateGameObjectCommand final : public EditorCommand
    {
    public:
        CreateGameObjectCommand(SceneSystem& sceneSystem, GameObject* parent = nullptr, string name = "Game Object");

        void execute() override;
        void undo() override;

        GameObject* createdObject() const;

    private:
        SceneSystem* m_sceneSystem = nullptr;

        uuid::Uuid m_objectId;

        std::optional<uuid::Uuid> m_parentId;

        string m_name;
    };

}
