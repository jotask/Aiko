#pragma once

#include <vector>

#include "aiko_types.h"
#include "layers/layer.h"

#include "windows/window.h"

#include "core/editor_context.h"

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
        using Windows = std::vector<AikoUPtr<Window>>;

        AikoEditor();

        const Windows& getWindows() { return m_windows; }

        EditorContext& context() { return m_context; }
        const EditorContext& context() const { return m_context; }

        RenderSystem* renderSystem() const { return m_renderSystem; }
        SceneSystem* sceneSystem() const { return m_sceneSystem; }

        void requestClose();

    protected:
        void connect(SystemConnector& connector) override;

        virtual void init() override;
        virtual void render() override;
    private:
        RenderSystem* m_renderSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;

        EditorContext m_context;
        Windows m_windows;

    };

}

