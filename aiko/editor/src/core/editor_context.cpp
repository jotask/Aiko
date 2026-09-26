#include "editor_context.h"

#include "systems/render_system.h"
#include "systems/scene_system.h"

namespace aiko::editor
{

    void EditorContext::connect(RenderSystem& renderSystem, SceneSystem& sceneSystem)
    {
        m_renderSystem = &renderSystem;
        m_sceneSystem = &sceneSystem;
    }

    RenderSystem& EditorContext::renderSystem()
    {
        AIKO_ASSERT(m_renderSystem != nullptr, "EditorContext has no RenderSystem");
        return *m_renderSystem;
    }

    const RenderSystem& EditorContext::renderSystem() const
    {
        AIKO_ASSERT(m_renderSystem != nullptr, "EditorContext has no RenderSystem");
        return *m_renderSystem;
    }

    SceneSystem& EditorContext::sceneSystem()
    {
        AIKO_ASSERT(m_sceneSystem != nullptr, "EditorContext has no SceneSystem");
        return *m_sceneSystem;
    }

    const SceneSystem& EditorContext::sceneSystem() const
    {
        AIKO_ASSERT(m_sceneSystem != nullptr, "EditorContext has no SceneSystem");
        return *m_sceneSystem;
    }

}
