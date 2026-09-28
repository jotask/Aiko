#pragma once

#include "layers/contexts/input_context.h"
#include "models/material.h"
#include "models/mesh.h"
#include "simulation.h"
#include "types/draw_types.h"

#include <array>
#include <deque>
#include <optional>

namespace aiko
{
    class RenderContext;
    class RenderSystem;
}

namespace sph
{

    class Playground
    {
    public:

        Playground();
        ~Playground();

        void init(const aiko::AssetId& shaderId);
        void update(const aiko::InputContext& input, const aiko::vec3& mousePosition);
        void render(aiko::RenderContext& renderer, aiko::RenderSystem& renderSystem);

    private:

        Simulation m_simulation;

        aiko::Mesh m_particleMesh;
        aiko::Material m_particleMaterial;

        struct ShapeRenderData
        {
            aiko::Mesh mesh;
            aiko::Material material;
        };

        std::deque<ShapeRenderData> m_shapeRenderData;

        aiko::vector<aiko::InstanceData> m_particleInstances;

        std::optional<size_t> m_selectedShape = std::nullopt;
        aiko::vec3 m_previousMousePosition = {};

    };

}
