#include "chunk_cellular_automaton.h"

#include <chrono>
#include <execution>

#include <models/game_object.h>
#include <systems/render_system.h>

#include "world_cellular_automaton.h"

namespace aiko::ca
{

    ChunkCellularAutomaton::ChunkCellularAutomaton(WorldCellularAutomaton* world, const ivec2 pos)
        : world(world)
        , pos(pos)
    {

    }

    void ChunkCellularAutomaton::init()
    {
        const auto isDebugCell = [](const uint x, const uint y) -> bool
        {
            if (x == 0 && y == 0)
            {
                return true;
            }
            if (x == SIZE_CHUNK.x - 1 && y == 0)
            {
                return true;
            }
            if (x == 0 && y == SIZE_CHUNK.y - 1)
            {
                return true;
            }
            if (x == SIZE_CHUNK.x - 1 && y == SIZE_CHUNK.y - 1)
            {
                return true;
            }
            return false;
        };
        cells.clear();
        cells.reserve(SIZE_CHUNK.product());
        for (int y = 0 ; y < SIZE_CHUNK.y; y++)
        {
            for (int x = 0; x < SIZE_CHUNK.x; x++)
            {
                if (DEBUG_CHUNKS == true && isDebugCell(x, y) == true)
                {
                    cells.push_back({ this, ivec2(x, y), CellCellularAutomaton::CellState::DEBUG });
                }
                else
                {
                    cells.push_back({ this, ivec2(x, y) });
                }
            }
        }
        std::for_each(cells.begin(), cells.end(), [](CellCellularAutomaton& cell) { cell.init(); });
    }

    void ChunkCellularAutomaton::preUpdate()
    {
        std::for_each(cells.begin(), cells.end(), [](CellCellularAutomaton& cell) { cell.preUpdate();} );
    }

    void ChunkCellularAutomaton::update()
    {
        std::for_each(cells.begin(), cells.end(), [](CellCellularAutomaton& cell){cell.update(); });
    }

    CellCellularAutomaton* ChunkCellularAutomaton::getCell(const ivec2 pos)
    {
        if (pos.x < 0 || pos.y < 0 || pos.x >= SIZE_CHUNK.x || pos.y >= SIZE_CHUNK.y)
        {
            return nullptr;
        }
        return &cells[getIndex(pos.x, pos.y, SIZE_CHUNK.x)];
    }

    WorldCellularAutomaton* ChunkCellularAutomaton::getWorld()
    {
        return world;
    }

    void ChunkCellularAutomaton::updateNeighbours()
    {
        std::for_each(cells.begin(), cells.end(), [](CellCellularAutomaton& cell){cell.updateNeighbours(); });
    }

}
