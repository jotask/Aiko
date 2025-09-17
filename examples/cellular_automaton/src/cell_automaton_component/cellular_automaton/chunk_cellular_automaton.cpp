#include "chunk_cellular_automaton.h"

#include <assert.h>
#include <chrono>
#include <execution>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

#include "world_cellular_automaton.h"

#include "core/log.h"

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
            if (x == cellautomaton::SIZE_CHUNK.x - 1 && y == 0)
            {
                return true;
            }
            if (x == 0 && y == cellautomaton::SIZE_CHUNK.y - 1)
            {
                return true;
            }
            if (x == cellautomaton::SIZE_CHUNK.x - 1 && y == cellautomaton::SIZE_CHUNK.y - 1)
            {
                return true;
            }
            return false;
        };
        cells.clear();
        cells.reserve(cellautomaton::SIZE_CHUNK.product());
        for (int y = 0 ; y < cellautomaton::SIZE_CHUNK.y; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_CHUNK.x; x++)
            {
                if (cellautomaton::DEBUG_CHUNKS == true && isDebugCell(x, y) == true)
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

    void ChunkCellularAutomaton::update()
    {
        if (cellautomaton::ASYNC_UPDATE_CELL == true)
        {
            std::for_each(std::execution::par, cells.begin(), cells.end(), [](CellCellularAutomaton& cell) { cell.preUpdate(); });
            std::for_each(std::execution::par, cells.begin(), cells.end(), [](CellCellularAutomaton& cell) { cell.update(); });
            return;
        }
        std::for_each(cells.begin(), cells.end(), [](CellCellularAutomaton& cell) { cell.preUpdate(); });
        std::for_each(cells.begin(), cells.end(), [](CellCellularAutomaton& cell) { cell.update(); });
    }

    CellCellularAutomaton* ChunkCellularAutomaton::getCell(const ivec2 pos)
    {
        // FIXME instead of std::find, we can convert the cell index from 2D to 1D dimension array
        auto found = std::find_if(cells.begin(), cells.end(), [pos](CellCellularAutomaton& cell) { return cell.getPosition() == pos; });
        if (found != cells.end())
        {
            return &(*found);
        }
        return nullptr;
    }

    WorldCellularAutomaton* ChunkCellularAutomaton::getWorld()
    {
        return world;
    }

}
