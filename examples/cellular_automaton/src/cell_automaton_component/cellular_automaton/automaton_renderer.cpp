#include "automaton_renderer.h"

#include "chunk_cellular_automaton.h"
#include "models/mesh_factory.h"
#include "world_cellular_automaton.h"

#include <layers/layer_context.h>
#include <types/draw_types.h>

#include <aiko_includes.h>

#include <vector>

namespace aiko::ca
{
    AutomatonRender::AutomatonRender()
    {

    }

    void AutomatonRender::init(aiko::LayerContext& context)
    {
        m_context = &context;

        m_mesh.upload(aiko::mesh::factory::generateQuad());

        m_material.m_shaderId = context.assets().loadShader("model");
        m_material.m_baseColor = aiko::WHITE;
        m_material.m_useVertexColor = true;
        m_material.m_lit = false;
        m_material.m_renderState.cullMode = CullMode::None;

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
        AIKO_ASSERT(chunk != nullptr, "Cannot render null automaton chunk");

        if (chunk == nullptr)
        {
            return;
        }

        const ivec2 chunkPosition = chunk->getPosition();

        const ivec2 chunkOrigin =
        {
            chunkPosition.x * SIZE_CHUNK.x,
            chunkPosition.y * SIZE_CHUNK.y
        };

        aiko::vector<aiko::InstanceData> instances;
        instances.reserve(SIZE_CHUNK.product());

        for (int y = 0; y < SIZE_CHUNK.y; ++y)
        {
            for (int x = 0; x < SIZE_CHUNK.x; ++x)
            {
                CellCellularAutomaton* cell = chunk->getCell({x, y});

                if (cell == nullptr)
                {
                    logger::Log::error("Cell out of bounds?");
                    continue;
                }

                const CellCellularAutomaton::CellState state = cell->getState();

                if (state == CellCellularAutomaton::CellState::NULLPTR)
                {
                    logger::Log::error("Invalid cell state");
                    continue;
                }

                if constexpr (DRAW_DEAD_CELLS == false)
                {
                    if (state == CellCellularAutomaton::CellState::DEAD)
                    {
                        continue;
                    }
                }

                const ivec2 cellPosition =
                {
                    chunkOrigin.x + x,
                    chunkOrigin.y + y
                };

                instances.push_back(
                    {
                        .position =
                        {
                            static_cast<float>(cellPosition.x),
                            static_cast<float>(cellPosition.y),
                            0.0f
                        },
                        .rotation = {0.0f, 0.0f, 0.0f},
                        .scale = {1.0f, 1.0f, 1.0f},
                        .color = getColorFromCell(state)
                    }
                );
            }
        }

        if (instances.empty())
        {
            return;
        }

        m_context->render().drawMeshInstanced(m_mesh, m_material, instances.data(), static_cast<u32>(instances.size()));

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
