#include "automaton_chunk_renderer.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

#include "chunk_cellular_automaton.h"
#include "world_cellular_automaton.h"

namespace aiko
{
    namespace cellautomaton
    {
        AutomatonRender::AutomatonRender()
            : m_renderSystem(nullptr)
            , m_mesh(nullptr)
            , m_shader(nullptr)
            , m_texture()
        {

        }

        void AutomatonRender::init(RenderSystem* renderSystem)
        {
            m_renderSystem = renderSystem;
            m_mesh = m_renderSystem->createMesh(Mesh::MeshType::QUAD);
            m_shader = m_renderSystem->createShader(global::getAssetPath("shaders/aiko_default_texture").c_str());
            m_texture.width = cellautomaton::SIZE_CHUNK;
            m_texture.height = cellautomaton::SIZE_CHUNK;
        }

        void AutomatonRender::update()
        {

        }

        void AutomatonRender::render(WorldCellularAutomaton* world)
        {
            for (auto& chunk : world->getChunks())
            {
                drawChunk(&chunk);
            }
        }
        void AutomatonRender::drawChunk(ChunkCellularAutomaton* chunk)
        {
            Transform trans;
            trans.position = chunk->getPosition();
            m_renderSystem->render(&trans, m_mesh.get(), m_shader.get(), &m_texture);
        }
    }
}
