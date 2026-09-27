#pragma once

#include "simulation.h"

#include "models/material.h"
#include "models/mesh.h"
#include "types/draw_types.h"

#include <array>

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
        void update();
        void render(aiko::RenderContext& renderer, aiko::RenderSystem& renderSystem);

    private:

        Simulation m_simulation;

        aiko::Mesh m_particleMesh;
        aiko::Material m_particleMaterial;

        std::array<aiko::InstanceData, Simulation::N_PARTICLES> m_particleInstances;

    };

}
