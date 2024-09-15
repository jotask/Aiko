#include "automaton_chunk_renderer.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "modules/render/2d_renderer.h"
#include "constants.h"

#include "chunk_cellular_automaton.h"
#include "world_cellular_automaton.h"

namespace aiko
{
    namespace cellautomaton
    {
        AutomatonRender::AutomatonRender()
            : m_renderSystem(nullptr)
        {

        }

        void AutomatonRender::init(RenderSystem* renderSystem)
        {
            m_renderSystem = renderSystem;
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
        }
    }
}
