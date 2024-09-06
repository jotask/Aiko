#pragma once

#include "aiko_types.h"
#include "types/color.h"

#include "components/2d/cellular_automaton/cell_cellular_automaton_helper.h"

#include "models/mesh.h"
#include "models/shader.h"

namespace aiko
{
    class WorldCellularAutomaton;
    class ChunkCellularAutomaton;
    namespace cellautomaton
    {

        class AutomatonRender
        {
            RenderSystem* m_renderSystem;
            AikoPtr<Mesh>       m_mesh;
            AikoPtr<Shader>     m_shader;
            texture::Texture    m_texture;

        public:

            AutomatonRender();
            ~AutomatonRender() = default;

            void init(RenderSystem* m_renderSystem);

            void update();

            void render(WorldCellularAutomaton* world);

            void drawChunk(ChunkCellularAutomaton* chunk);

        };

    }
}