#pragma once

#include "contexts/render_context.h"
#include "layers/contexts/input_context.h"

#include <aiko_types.h>

namespace aiko
{
    class GameObject;
    class SceneSystem;
    class AssetSystem;
    class SystemConnector;

    class LayerContext
    {
    public:
        explicit LayerContext(SystemConnector&);

        GameObject* Instantiate(string name);
        GameObject* Instantiate(GameObject* parent, string name);

        float getDeltaTime() const;

        AssetId loadShader(string_view source);

        InputContext& input() { return m_input; }
        const InputContext& input() const { return m_input; }

        RenderContext& render() { return m_render; }
        const RenderContext& render() const { return m_render; }

    private:
        SceneSystem* m_sceneSystem = nullptr;
        AssetSystem* m_assetSystem = nullptr;

        InputContext m_input;
        RenderContext m_render;

    };
}
