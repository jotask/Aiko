#include "automaton_chunk_renderer.h"

#include <assert.h>
#include <chrono>

#include <aiko_includes.h>

#include "chunk_cellular_automaton.h"
#include "world_cellular_automaton.h"

namespace aiko::ca
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

        void AutomatonRender::render(WorldCellularAutomaton* world)
        {
            for (auto& chunk : world->getChunks())
            {
                drawChunk(&chunk);
            }
        }

        void AutomatonRender::drawChunk(ChunkCellularAutomaton* chunk)
        {
            auto ctx = m_renderSystem->getRenderer<RenderContext2D>(ContextType::Render2D);
            auto cam = m_renderSystem->getMainCamera();
            vec2 chunkPosition = chunk->getPosition() * cellautomaton::SIZE_CHUNK;
            if (s_render_cells == false)
            {
                Color randomColor = Color::getRandomColor();
                randomColor.a = 0.25f;
                ctx->drawRectangle(cam, chunkPosition, { cellautomaton::SIZE_CHUNK , cellautomaton::SIZE_CHUNK }, randomColor );
                return;
            }
            for (int y = 0; y < cellautomaton::SIZE_CHUNK; y++)
            {
                for (int x = 0; x < cellautomaton::SIZE_CHUNK; x++)
                {
                    const ChunkCellularAutomaton::CellState cellState = chunk->getCell({ x, y });
                    if (cellState == ChunkCellularAutomaton::CellState::NULLPTR)
                    {
                        Log::error("Cell out fo bounds?");
                        continue;
                    }
                    if (DRAW_DEAD_CELLS == false && cellState == ChunkCellularAutomaton::CellState::DEAD)
                    {
                        continue;
                    }
                    vec2 cellPosition = {static_cast<float>(x), static_cast<float>(y) };
                    const auto color = chunk->getColorFromCell(cellState);
                    ctx->drawRectangle(cam, chunkPosition + cellPosition, { 1 , 1 }, color);
                }
            }
        }
    }
}
