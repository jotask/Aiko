#include "world_cellular_automaton.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

namespace aiko
{

    WorldCellularAutomaton::WorldCellularAutomaton()
    {

    }

    void WorldCellularAutomaton::init()
    {
        for (int y = 0; y < cellautomaton::SIZE_WORLD; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_WORLD; x++)
            {
                m_chunks.push_back(ChunkCellularAutomaton(this, x, y));
            }
        }
    }

    void WorldCellularAutomaton::update()
    {
        for (auto& c : m_chunks)
        {
            c.update();
        }
    }

    void WorldCellularAutomaton::render()
    {
        for (auto& c : m_chunks)
        {
            c.render();
        }
    }

    WorldCellularAutomaton::Chunks WorldCellularAutomaton::getChunks()
    {
        return m_chunks;
    }

}
