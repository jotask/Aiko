#include "chunk_cellular_automaton.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

#include "world_cellular_automaton.h"

namespace aiko
{

    ChunkCellularAutomaton::ChunkCellularAutomaton(WorldCellularAutomaton* world, const int x, const int y)
        : world(world)
        , x(x)
        , y(y)
    {
        init();
    }

    void ChunkCellularAutomaton::init()
    {
        for (int y = 0 ; y < cellautomaton::SIZE_CHUNK; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_CHUNK; x++)
            {
                m_cells.emplace_back(this, x, y);
            }
        }

        if(false)
        {
            m_cells[cellautomaton::getIndex(0, 0, cellautomaton::SIZE_CHUNK)].setState(CellCellularAutomaton::CellState::DEBUG);
            m_cells[cellautomaton::getIndex(cellautomaton::SIZE_CHUNK - 1, 0, cellautomaton::SIZE_CHUNK)].setState(CellCellularAutomaton::CellState::DEBUG);
            m_cells[cellautomaton::getIndex(0, cellautomaton::SIZE_CHUNK - 1, cellautomaton::SIZE_CHUNK)].setState(CellCellularAutomaton::CellState::DEBUG);
            m_cells[cellautomaton::getIndex(cellautomaton::SIZE_CHUNK - 1, cellautomaton::SIZE_CHUNK - 1, cellautomaton::SIZE_CHUNK)].setState(CellCellularAutomaton::CellState::DEBUG);
        }

        if (true)
        {
            for (auto& c : m_cells)
            {
                auto rnd = utils::getRandomValue(0.0f, 1.0f);
                if (rnd < 0.5f)
                {
                    c.setState(CellCellularAutomaton::CellState::LIVE);
                }
                else
                {
                    c.setState(CellCellularAutomaton::CellState::DEAD);
                }
            }
        }

    }

    void ChunkCellularAutomaton::update()
    {
        for (auto& cell : m_cells)
        {
            cell.update();
        }
    }

    void ChunkCellularAutomaton::render()
    {
        for (auto& cell : m_cells)
        {

        }
    }

    std::vector<Color> ChunkCellularAutomaton::getPixels()
    {
        std::vector<Color> colors;
        for (int y = 0; y < cellautomaton::SIZE_CHUNK; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_CHUNK; x++)
            {
                const auto idx = cellautomaton::getIndex(x, y, cellautomaton::SIZE_CHUNK);
                colors.push_back(m_cells[idx].getColor());
            }
        }
        return colors;
    }

    ChunkCellularAutomaton::Cells& ChunkCellularAutomaton::getCells()
    {
        return m_cells;
    }

    std::vector<CellCellularAutomaton> ChunkCellularAutomaton::getNeighbours(ivec2 cell)
    {
        return world->getNeighbours({x, y}, cell);
    }

    CellCellularAutomaton* ChunkCellularAutomaton::getCell(const ivec2 pos)
    {
        auto found = std::find_if(m_cells.begin(), m_cells.end(), [pos](CellCellularAutomaton& c)
        {
            return c.getPosition() == pos;
        });
        if (found == m_cells.end())
        {
            return nullptr;
        }
        return &(*found);
    }

}
