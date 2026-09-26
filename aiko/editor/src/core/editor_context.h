#pragma once

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

        RenderSystem& renderSystem();
        const RenderSystem& renderSystem() const;

        SceneSystem& sceneSystem();
        const SceneSystem& sceneSystem() const;

        GameObject* selectedGameObject() const
        {
            return m_selectedGameObject;
        }

        void select(GameObject* object)
        {
            m_selectedGameObject = object;
        }

        void clearSelection()
        {
            m_selectedGameObject = nullptr;
        }

    private:
        RenderSystem* m_renderSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;

        GameObject* m_selectedGameObject = nullptr;
    };

}
