#pragma once

#include "commands/editor_command_stack.h"
#include "core/editor_view_settings.h"
#include "core/editor_document.h"

namespace aiko
{
    class GameObject;
    class RenderSystem;
    class SceneSystem;
    class AssetSystem;
}

namespace aiko::editor
{

    class EditorContext
    {
    public:
        EditorContext() = default;

        void connect(RenderSystem& renderSystem, SceneSystem& sceneSystem, AssetSystem& assetSystem);

        EditorCommandStack& commands() { return m_commands; }
        const EditorCommandStack& commands() const { return m_commands; }

        RenderSystem& renderSystem();
        const RenderSystem& renderSystem() const;

        SceneSystem& sceneSystem();
        const SceneSystem& sceneSystem() const;

        AssetSystem& assetSystem();
        const AssetSystem& assetSystem() const;

        GameObject* selectedGameObject() const { return m_selectedGameObject; }
        void select(GameObject* object) { m_selectedGameObject = object; }
        void clearSelection() { m_selectedGameObject = nullptr; }

        EditorViewSettings& viewSettings() { return m_viewSettings; }
        const EditorViewSettings& viewSettings() const { return m_viewSettings; }

        EditorDocument& document() { return m_document; }
        const EditorDocument& document() const { return m_document; }

    private:

        EditorCommandStack m_commands;

        EditorViewSettings m_viewSettings;
        EditorDocument m_document;

        RenderSystem* m_renderSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;
        AssetSystem* m_assetSystem = nullptr;

        GameObject* m_selectedGameObject = nullptr;
    };

}
