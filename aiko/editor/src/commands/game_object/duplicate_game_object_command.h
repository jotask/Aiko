#pragma once

#include <optional>

#include <aiko_types.h>
#include <yaml-cpp/yaml.h>

#include "commands/editor_command.h"
#include "core/uuid.h"

namespace aiko
{
    class GameObject;
    class SceneSystem;
}

namespace aiko::editor
{

    class DuplicateGameObjectCommand final : public EditorCommand
    {
    public:
        DuplicateGameObjectCommand(SceneSystem& sceneSystem, const GameObject& source);

        void execute() override;
        void undo() override;

        GameObject* duplicatedObject() const;

    private:
        SceneSystem* m_sceneSystem = nullptr;

        uuid::Uuid m_duplicateId;
        std::optional<uuid::Uuid> m_parentId;

        string m_name;
        bool m_active = true;

        vector<YAML::Node> m_components;
    };

}
