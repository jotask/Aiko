#pragma once

#include "layers/layer.h"

#include "core/editor_context.h"
#include "core/editor_workspace.h"

namespace aiko
{
    class RenderSystem;
    class SceneSystem;
    class SystemConnector;
}

namespace aiko::editor
{

    class AikoEditor : public Layer
    {
    public:
        AikoEditor() = default;

        EditorContext& context()
        {
            return m_context;
        }

        const EditorContext& context() const
        {
            return m_context;
        }

        EditorWorkspace& workspace()
        {
            return m_workspace;
        }

        const EditorWorkspace& workspace() const
        {
            return m_workspace;
        }

    protected:
        void connect(SystemConnector& connector) override;
        void init() override;
        void render() override;

    private:
        RenderSystem* m_renderSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;

        EditorContext m_context;
        EditorWorkspace m_workspace;
    };

}