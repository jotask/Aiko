#pragma once

#include "commands/transform/transform_state.h"
#include "core/editor_panel.h"
#include <core/transform.h>

#include <models/camera.h>
#include <models/render_target.h>

namespace aiko::editor
{

    class SceneViewPanel final : public EditorPanel
    {
    public:
        SceneViewPanel();

        void render(EditorContext& context) override;

    private:

        static TransformState captureTransform(const Transform& transform)
        {
            return
            {
                transform.position,
                transform.rotation,
                transform.scale
            };
        }

        Camera m_camera;
        RenderTarget m_renderTarget;

        TransformState m_gizmoStartTransform;
        bool m_wasUsingGizmo = false;
    };

}
