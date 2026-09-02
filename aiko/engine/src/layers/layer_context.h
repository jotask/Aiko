#pragma once

#include "layers/contexts/render_context.h"
#include "layers/contexts/input_context.h"
#include "layers/contexts/scene_context.h"
#include "layers/contexts/asset_context.h"

namespace aiko
{

    class Aiko;
    class SystemConnector;

    class LayerContext
    {
    public:

        float getDeltaTime() const;

        InputContext& input() { return m_input; }
        const InputContext& input() const { return m_input; }

        RenderContext& render() { return m_render; }
        const RenderContext& render() const { return m_render; }

        SceneContext& scene() { return m_scene; }
        const SceneContext& scene() const { return m_scene; }

        AssetContext& assets() { return m_assets; }
        const AssetContext& assets() const { return m_assets; }

    private:

        friend class Aiko;

        explicit LayerContext(SystemConnector&);

        InputContext m_input;
        RenderContext m_render;
        SceneContext m_scene;
        AssetContext m_assets;

    };
}
