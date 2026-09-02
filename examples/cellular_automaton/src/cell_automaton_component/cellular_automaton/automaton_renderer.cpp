#include "automaton_renderer.h"

#include <aiko_includes.h>
#include <layers/layer_context.h>

#include "chunk_cellular_automaton.h"
#include "world_cellular_automaton.h"

namespace aiko::ca
{
    namespace cellautomaton
    {
        AutomatonRender::AutomatonRender()
        {

        }

        void AutomatonRender::init(aiko::LayerContext& context)
        {
            m_context = &context;
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

            ivec2 chunkPosition = chunk->getPosition();
            if (s_render_cells == false)
            {
                Color randomColor = Color::getRandomColor();
                randomColor.a = 1.0f;

                const vec3 position =
                {
                    static_cast<float>(chunkPosition.x),
                    static_cast<float>(chunkPosition.y),
                    0.0f
                };

                const vec3 size =
                {
                    static_cast<float>(cellautomaton::SIZE_CHUNK.x),
                    static_cast<float>(cellautomaton::SIZE_CHUNK.y),
                    1.0f
                };

                m_context->render().drawRectangle(position, size);

                return;
            }

            for (int y = 0; y < cellautomaton::SIZE_CHUNK.y; y++)
            {
                for (int x = 0; x < cellautomaton::SIZE_CHUNK.x; x++)
                {
                    CellCellularAutomaton* cell = chunk->getCell({ x, y });
                    const CellCellularAutomaton::CellState cellState = cell->getState();
                    if (cellState == CellCellularAutomaton::CellState::NULLPTR)
                    {
                        logger::Log::error("Cell out fo bounds?");
                        continue;
                    }
                    if (DRAW_DEAD_CELLS == false && cellState == CellCellularAutomaton::CellState::DEAD)
                    {
                        continue;
                    }
                    const ivec2 cellPosition = { x, y };
                    const ivec2 position2D = chunkPosition + cellPosition;

                    const vec3 position =
                    {
                        static_cast<float>(position2D.x),
                        static_cast<float>(position2D.y),
                        0.0f
                    };

                    m_context->render().drawRectangle(position, { 1.0f, 1.0f, 1.0f });
                }
            }
        }

        Color AutomatonRender::getColorFromCell(CellCellularAutomaton::CellState stat)
        {
            switch (stat)
            {
                case CellCellularAutomaton::CellState::LIVE:    return WHITE;
                case CellCellularAutomaton::CellState::DEAD:    return BLACK;
                case CellCellularAutomaton::CellState::DEBUG:   return MAGENTA;
                case CellCellularAutomaton::CellState::NULLPTR: return MAGENTA;
                default:                         assert(false); return MAGENTA;
            }
        }

    }
}
