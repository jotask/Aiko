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
    {
        init();
    }

    void ChunkCellularAutomaton::init()
    {

        for (int y = 0 ; y < cellautomaton::SIZE_CHUNK; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_CHUNK; x++)
            {
                auto idx = cellautomaton::getIndex( x, y, cellautomaton::SIZE_CHUNK );
                cells[idx] = CellState::DEAD;
            }
        }

        if(false)
        {
            cells[cellautomaton::getIndex(0, 0, cellautomaton::SIZE_CHUNK)] = CellState::DEBUG;
            cells[cellautomaton::getIndex(cellautomaton::SIZE_CHUNK - 1, 0, cellautomaton::SIZE_CHUNK)] = CellState::DEBUG;
            cells[cellautomaton::getIndex(0, cellautomaton::SIZE_CHUNK - 1, cellautomaton::SIZE_CHUNK)] = CellState::DEBUG;
            cells[cellautomaton::getIndex(cellautomaton::SIZE_CHUNK - 1, cellautomaton::SIZE_CHUNK - 1, cellautomaton::SIZE_CHUNK)] = CellState::DEBUG;
        }

        if (true)
        {
            for (auto& c : cells)
            {
                auto rnd = utils::getRandomValue(0.0f, 1.0f);
                if (rnd < 0.5f)
                {
                    c = CellState::LIVE;
                }
                else
                {
                    c = CellState::DEAD;
                }
            }
        }

        prev_cells = cells;

    }

    void ChunkCellularAutomaton::update()
    {
        prev_cells = cells;
        for (int y = 0; y < cellautomaton::SIZE_CHUNK; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_CHUNK; x++)
            {

                const auto idx = cellautomaton::getIndex(x, y, cellautomaton::SIZE_CHUNK);
                auto state = prev_cells[idx];
                
                std::vector<CellState> neighbours = getNeighbours({ x, y });

                const auto alive = std::count_if(neighbours.begin(), neighbours.end(), [](CellState& c)
                {
                    return c == CellState::LIVE;
                });

                if (state == ChunkCellularAutomaton::CellState::LIVE)
                {
                    // Loneliness: A live cell with fewer than 2 live neighbors dies
                    // Overcrowding: A live cell with more than 3 live neighbors dies
                    if (alive < 2 || alive > 3)
                    {
                        state = CellState::DEAD;
                    }
                    // Survival: A live cell with 2 or 3 live neighbors stays alive (no state change needed here)
                }
                else if (state == CellState::DEAD)
                {
                    // Reproduction: A dead cell with exactly 3 live neighbors becomes alive
                    if (alive == 3)
                    {
                        state = CellState::LIVE;
                    }
                }
                cells[idx] = state;
            }
        }
    }

    std::vector<ChunkCellularAutomaton::CellState> ChunkCellularAutomaton::getNeighbours(ivec2 cell)
    {
        return world->getNeighbours(pos, cell);
    }

    ChunkCellularAutomaton::CellState ChunkCellularAutomaton::getCell(const ivec2 pos)
    {
        const auto idx = cellautomaton::getIndex(pos.x, pos.y, cellautomaton::SIZE_CHUNK);
        if (idx >= 0 && idx < cellautomaton::SIZE_CHUNK * cellautomaton::SIZE_CHUNK)
        {
            return prev_cells[idx];
        }
        return  ChunkCellularAutomaton::CellState::NULLPTR;

    }

    Color ChunkCellularAutomaton::getColorFromCell(CellState stat)
    {
        switch (stat)
        {
            case CellState::LIVE:  return WHITE;
            case CellState::DEAD:  return BLACK;
            case CellState::DEBUG: return MAGENTA;
            default:               return MAGENTA;
        }
    }

}
