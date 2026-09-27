#pragma once

#include "commands/editor_command_stack.h"
#include "core/editor_view_settings.h"

namespace aiko
{
    class GameObject;
    class RenderSystem;
    class SceneSystem;
}

namespace aiko::editor
{

    class EditorContext
    {
    public:
        EditorContext() = default;

        void connect(RenderSystem& renderSystem, SceneSystem& sceneSystem);

        EditorCommandStack& commands() { return m_commands; }
        const EditorCommandStack& commands() const { return m_commands; }

        RenderSystem& renderSystem();
        const RenderSystem& renderSystem() const;

        SceneSystem& sceneSystem();
        const SceneSystem& sceneSystem() const;

        GameObject* selectedGameObject() const { return m_selectedGameObject; }
        void select(GameObject* object) { m_selectedGameObject = object; }
        void clearSelection() { m_selectedGameObject = nullptr; }

        EditorViewSettings& viewSettings() { return m_viewSettings; }
        const EditorViewSettings& viewSettings() const { return m_viewSettings; }

    private:

        EditorCommandStack m_commands;

        EditorViewSettings m_viewSettings;

        RenderSystem* m_renderSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;

        GameObject* m_selectedGameObject = nullptr;
    };

}
