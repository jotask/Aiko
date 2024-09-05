#include "world_cellular_automaton.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

namespace aiko
{

    WorldCellularAutomaton::WorldCellularAutomaton()
        // : m_chunks(cellautomaton::SIZE_WORLD == 1 ? 1 : cellautomaton::SIZE_WORLD * cellautomaton::SIZE_WORLD)
    {

    }

    void WorldCellularAutomaton::init()
    {
        for (int y = 0; y < cellautomaton::SIZE_WORLD; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_WORLD; x++)
            {
                m_chunks.emplace_back(this, x, y);
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

    WorldCellularAutomaton::Chunks& WorldCellularAutomaton::getChunks()
    {
        return m_chunks;
    }

    std::vector<CellCellularAutomaton> WorldCellularAutomaton::getNeighbours(ivec2 chunk, ivec2 cell)
    {
        auto isCurrentChunk = [&](ivec2 p) -> bool
        {
            return ( p.x >= 0 && p.x < cellautomaton::SIZE_CHUNK ) && (p.y >= 0 && p.y < cellautomaton::SIZE_CHUNK);
        };

        constexpr const int NEIGHBOUR = 1;
        auto neighbours = std::vector<CellCellularAutomaton>();

        for (int y = -NEIGHBOUR; y <= NEIGHBOUR; y++)
        {
            for (int x = -NEIGHBOUR; x <= NEIGHBOUR; x++)
            {
                if (y == 0 && x == 0)
                {
                    continue;
                }
                // Calculate the position of the neighboring cell
                ivec2 pos = { cell.x + x, cell.y + y };
                ivec2 targetChunk = chunk; // Start with the current chunk

                // Check if the position falls within the current chunk
                if ( isCurrentChunk(pos) == false )
                {
                    // Adjust chunk coordinates based on the cell's offset
                    if (pos.x < 0)
                    {
                        targetChunk.x -= 1; // Move to the left chunk
                        pos.x += cellautomaton::SIZE_CHUNK; // Wrap around to the last cell in the left chunk
                    }
                    else if (pos.x >= cellautomaton::SIZE_CHUNK)
                    {
                        targetChunk.x += 1; // Move to the right chunk
                        pos.x -= cellautomaton::SIZE_CHUNK; // Wrap around to the first cell in the right chunk
                    }

                    if (pos.y < 0)
                    {
                        targetChunk.y -= 1; // Move to the chunk above
                        pos.y += cellautomaton::SIZE_CHUNK; // Wrap around to the last cell in the above chunk
                    }
                    else if (pos.y >= cellautomaton::SIZE_CHUNK)
                    {
                        targetChunk.y += 1; // Move to the chunk below
                        pos.y -= cellautomaton::SIZE_CHUNK; // Wrap around to the first cell in the below chunk
                    }
                }

                // Retrieve the chunk based on the adjusted chunk position
                ChunkCellularAutomaton* chunkPtr = getChunk(targetChunk);
                if (chunkPtr != nullptr)
                {
                    // Retrieve the cell from the target chunk
                    CellCellularAutomaton* neighborCell = chunkPtr->getCell(pos);
                    if (neighborCell != nullptr)
                    {
                        neighbours.push_back(*neighborCell); // Add the neighbor cell to the vector
                    }
                }
            }
        }
        return neighbours;
    }

    ChunkCellularAutomaton* WorldCellularAutomaton::getChunk(const ivec2 pos)
    {
        auto found = std::find_if(m_chunks.begin(), m_chunks.end(), [pos](ChunkCellularAutomaton& c)
        {
            return c.getPosition() == pos;
        });
        if (found == m_chunks.end())
        {
            return nullptr;
        }
        return &(*found);
    }

}
