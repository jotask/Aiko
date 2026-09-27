#pragma once

#include "core/editor_panel.h"

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
        Camera m_camera;
        RenderTarget m_renderTarget;
    };

}
