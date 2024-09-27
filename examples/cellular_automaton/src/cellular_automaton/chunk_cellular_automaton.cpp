#include "chunk_cellular_automaton.h"

#include <assert.h>
#include <chrono>
#include <thread>

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
        , cells(cellautomaton::SIZE_CHUNK * cellautomaton::SIZE_CHUNK)
    {

    }

    void ChunkCellularAutomaton::init()
    {
        constexpr bool debug_cells = false;
        const auto isDebugCell = [](const uint x, const uint y) -> bool
        {
            if (x == 0 && y == 0)
            {
                return true;
            }
            if (x == cellautomaton::SIZE_CHUNK - 1 && y == 0)
            {
                return true;
            }
            if (x == 0 && y == cellautomaton::SIZE_CHUNK - 1)
            {
                return true;
            }
            if (x == cellautomaton::SIZE_CHUNK - 1 && y == cellautomaton::SIZE_CHUNK - 1)
            {
                return true;
            }
            return false;
        };
        cells.clear();
        cells.reserve(cellautomaton::SIZE_CHUNK * cellautomaton::SIZE_CHUNK);
        for (int y = 0 ; y < cellautomaton::SIZE_CHUNK; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_CHUNK; x++)
            {
                auto idx = cellautomaton::getIndex( x, y, cellautomaton::SIZE_CHUNK );
                if (debug_cells == true && isDebugCell(x, y) == true)
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
        std::for_each(cells.begin(), cells.end(), [](CellCellularAutomaton& cell) { cell.preUpdate(); });
        std::for_each(cells.begin(), cells.end(), [](CellCellularAutomaton& cell) { cell.update(); });
    }

    CellCellularAutomaton* ChunkCellularAutomaton::getCell(const ivec2 pos)
    {
        const uint idx = cellautomaton::getIndex(pos.x, pos.y, cellautomaton::SIZE_CHUNK);
        if (idx >= 0 && idx < cells.size())
        {
            return &cells[idx];
        }
        return nullptr;
    }

    WorldCellularAutomaton* ChunkCellularAutomaton::getWorld()
    {
        return world;
    }

}
