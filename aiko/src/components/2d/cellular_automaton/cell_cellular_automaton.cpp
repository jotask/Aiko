#include "cell_cellular_automaton.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

#include "chunk_cellular_automaton.h"

namespace aiko
{

    CellCellularAutomaton::CellCellularAutomaton(ChunkCellularAutomaton* chunk, const int i, const int j)
        : m_chunk(chunk)
        , i(i)
        , j(j)
        , state(CellState::DEAD)
    {
    }

    void CellCellularAutomaton::update()
    {
        std::vector<CellCellularAutomaton> neighbours = m_chunk->getNeighbours({i, j});

        auto find = [](CellCellularAutomaton& c)
        {
            return c.getState() == CellState::LIVE;
        };

        const auto alive = std::count_if( neighbours.begin(), neighbours.end(), find );

        if (state == CellState::LIVE)
        {
            // Loneliness: A live cell with fewer than 2 live neighbors dies
            if (alive < 2)
            {
                state = CellState::DEAD;
            }
            // Overcrowding: A live cell with more than 3 live neighbors dies
            else if (alive > 3)
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

    void CellCellularAutomaton::setState(CellState s)
    {
        state = s;
    }

    CellCellularAutomaton::CellState CellCellularAutomaton::getState()
    {
        return state;;
    }

    Color CellCellularAutomaton::getColor()
    {
        switch (state)
        {
        case aiko::CellCellularAutomaton::CellState::LIVE:  return WHITE;
        case aiko::CellCellularAutomaton::CellState::DEAD:  return BLACK;
        case aiko::CellCellularAutomaton::CellState::DEBUG: return MAGENTA;
        default:
            break;
        }
    }

    ChunkCellularAutomaton* CellCellularAutomaton::getChunk()
    {
        return m_chunk;
    }

}
