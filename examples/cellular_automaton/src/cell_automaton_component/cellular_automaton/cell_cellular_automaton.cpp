#include "cell_cellular_automaton.h"

#include <assert.h>
#include <chrono>
#include <thread>
#include <iostream>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

#include "world_cellular_automaton.h"

#include "core/log.h"

namespace aiko::ca
{

    CellCellularAutomaton::CellCellularAutomaton(ChunkCellularAutomaton* chunk, const ivec2 pos, CellState st)
        : chunk(chunk)
        , pos(pos)
        , state(st)
        , prev_state(state)
    {

    }

    void CellCellularAutomaton::init()
    {
        if (cellautomaton::RANDOM_CELL_INIT == true && state != CellState::DEBUG)
        {
            bool rnd = utils::getRandomBool();
            if (rnd)
            {
                state = CellState::LIVE;
            }
            else
            {
                state = CellState::DEAD;
            }
        }
        updateNeighbours();
        prev_state = state;
    }

    void CellCellularAutomaton::preUpdate()
    {
        prev_state = state;
    }

    void CellCellularAutomaton::update()
    {
        const auto alive = std::count_if(cache_neighbours.begin(), cache_neighbours.end(), [](CellCellularAutomaton* c)
        {
            return c->getPrevState() == CellState::LIVE;
        });
        if (state == CellState::LIVE)
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
    }

    void CellCellularAutomaton::updateNeighbours()
    {
        cache_neighbours = chunk->getWorld()->getNeighbours(chunk->getPosition(), pos);
    }

    void CellCellularAutomaton::setState(CellState st)
    {
        state = st;
        prev_state = state;
    }

    CellCellularAutomaton::CellState CellCellularAutomaton::getState()
    {
        return state;
    }

    CellCellularAutomaton::CellState CellCellularAutomaton::getPrevState()
    {
        return prev_state;
    }

    std::vector<CellCellularAutomaton*> CellCellularAutomaton::getCache()
    {
        return cache_neighbours;
    }

    ChunkCellularAutomaton* CellCellularAutomaton::getChunk()
    {
        return chunk;
    }

}
