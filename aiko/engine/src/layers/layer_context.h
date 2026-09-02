#pragma once

#include <aiko_types.h>
#include <math/math.h>
#include <input/inputs_types.h>

#include "layers/contexts/input_context.h"

namespace aiko
{
    class GameObject;
    class SceneSystem;
    class RenderSystem;
    class AssetSystem;
    class SystemConnector;
    class Mesh;
    class Material;

    class LayerContext
    {
    public:
        explicit LayerContext(SystemConnector&);

        GameObject* Instantiate(string name);
        GameObject* Instantiate(GameObject* parent, string name);

        float getDeltaTime() const;

        // Draw
        void drawRectangle(const vec3& position, const vec3& size);
        void drawMesh(const Transform& transform, const Mesh& mesh, const Material& material);

        AssetId loadShader(string_view source);

        InputContext& input() { return m_input; }
        const InputContext& input() const { return m_input; }

    private:
        SceneSystem* m_sceneSystem = nullptr;
        RenderSystem* m_renderSystem = nullptr;
        AssetSystem* m_assetSystem = nullptr;

        InputContext m_input;

    };
}
