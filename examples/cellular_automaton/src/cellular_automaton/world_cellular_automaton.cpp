#include "world_cellular_automaton.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "constants.h"

#include "core/log.h"

namespace aiko::ca
{

    WorldCellularAutomaton::WorldCellularAutomaton()
    {
    }

    void WorldCellularAutomaton::init()
    {
        m_chunks.clear();
        m_chunks.reserve(cellautomaton::SIZE_WORLD * cellautomaton::SIZE_WORLD);
        for (int y = 0; y < cellautomaton::SIZE_WORLD; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_WORLD; x++)
            {
                m_chunks.emplace_back(this, ivec2 { x, y });
            }
        }
        for (ChunkCellularAutomaton& c : m_chunks)
        {
            c.init();
        }
    }

    void WorldCellularAutomaton::update()
    {
        for (ChunkCellularAutomaton& c : m_chunks)
        {
            c.update();
        }
    }

    WorldCellularAutomaton::Chunks& WorldCellularAutomaton::getChunks()
    {
        return m_chunks;
    }

    std::vector<CellCellularAutomaton*> WorldCellularAutomaton::getNeighbours(ivec2 chunk, ivec2 cell)
    {

        static auto isCurrentChunk = [&](ivec2 p) -> bool
        {
            return ( p.x >= 0 && p.x < cellautomaton::SIZE_CHUNK ) && (p.y >= 0 && p.y < cellautomaton::SIZE_CHUNK);
        };

        auto neighbours = std::vector<CellCellularAutomaton*>();

        for (int y = -cellautomaton::NEIGHBOURS.y; y <= cellautomaton::NEIGHBOURS.y; ++y)
        {
            for (int x = -cellautomaton::NEIGHBOURS.x; x <= cellautomaton::NEIGHBOURS.x; ++x)
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
                    CellCellularAutomaton* neighborCell = chunkPtr->getCell(pos);
                    if (neighborCell != nullptr)
                    {
                        neighbours.push_back(neighborCell);
                    }
                }
            }
        }
        return neighbours;
    }

    ChunkCellularAutomaton* WorldCellularAutomaton::getChunk(const ivec2 pos)
    {
        const auto idx = cellautomaton::getIndex(pos.x, pos.y, cellautomaton::SIZE_WORLD);
        if ( idx >= 0 && idx < m_chunks.size())
        {
            return &m_chunks[idx];
        }
        return nullptr;
    }

}
