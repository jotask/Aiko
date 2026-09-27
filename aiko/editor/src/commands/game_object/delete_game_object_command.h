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

    class DeleteGameObjectCommand final : public EditorCommand
    {
    public:
        DeleteGameObjectCommand(SceneSystem& sceneSystem, GameObject& object);

        void execute() override;
        void undo() override;

    private:
        SceneSystem* m_sceneSystem = nullptr;

        uuid::Uuid m_objectId;
        std::optional<uuid::Uuid> m_parentId;

        string m_name;
        bool m_active = true;

        vector<YAML::Node> m_components;
        vector<uuid::Uuid> m_childIds;
    };

}
