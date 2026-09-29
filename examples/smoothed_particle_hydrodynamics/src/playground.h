#pragma once

#include "layers/contexts/input_context.h"
#include "models/material.h"
#include "models/mesh.h"
#include "simulation.h"
#include "sph_gpu_simulation.h"
#include "types/draw_types.h"

#include <array>
#include <deque>
#include <optional>

namespace aiko
{
    class AssetSystem;
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

        void init(const aiko::AssetId& shaderId, const aiko::AssetId& gpuShaderId, aiko::AssetSystem& assetSystem);
        void update(const aiko::InputContext& input, const aiko::vec3& mousePosition);
        void render(aiko::RenderContext& renderer, aiko::RenderSystem& renderSystem);

    private:

        enum class ParticleColorMode : uint32_t
        {
            Constant = 0,
            Velocity = 1,
            Pressure = 2,
        };

        Simulation m_simulation;
        SPHGpuSimulation m_gpuSimulation;

        aiko::Mesh m_particleMesh;
        aiko::Material m_particleMaterial;
        aiko::Material m_gpuParticleMaterial;

        struct ShapeRenderData
        {
            aiko::Mesh mesh;
            aiko::Material material;
        };

        std::deque<ShapeRenderData> m_shapeRenderData;

        aiko::vector<aiko::InstanceData> m_particleInstances;

        std::optional<size_t> m_selectedShape = std::nullopt;
        aiko::vec3 m_previousMousePosition = {};

        ParticleColorMode m_particleColorMode = ParticleColorMode::Velocity;

    };

}
