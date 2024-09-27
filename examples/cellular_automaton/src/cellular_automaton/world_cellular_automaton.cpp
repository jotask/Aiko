#include "world_cellular_automaton.h"

#include <assert.h>
#include <chrono>
#include <execution>

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
        m_chunks.reserve(cellautomaton::SIZE_WORLD.product());
        for (int y = 0; y < cellautomaton::SIZE_WORLD.y; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_WORLD.x; x++)
            {
                m_chunks.emplace_back(this, ivec2 { x, y });
            }
        }
        std::for_each(m_chunks.begin(), m_chunks.end(), [](ChunkCellularAutomaton& chunk) { chunk.init(); });
    }

    void WorldCellularAutomaton::update()
    {
        if (cellautomaton::ASYNC_UPDATE_CHUNK == true)
        {
            std::for_each(std::execution::par, m_chunks.begin(), m_chunks.end(), [](ChunkCellularAutomaton& chunk) { chunk.update(); });
            return;
        }
        std::for_each(m_chunks.begin(), m_chunks.end(), [](ChunkCellularAutomaton& chunk) { chunk.update(); });
    }

    WorldCellularAutomaton::Chunks& WorldCellularAutomaton::getChunks()
    {
        return m_chunks;
    }

    std::vector<CellCellularAutomaton*> WorldCellularAutomaton::getNeighbours(ivec2 chunk, ivec2 cell)
    {

        static auto isCurrentChunk = [&](ivec2 p) -> bool
        {
            return ( p.x >= 0 && p.x < cellautomaton::SIZE_CHUNK.x ) && (p.y >= 0 && p.y < cellautomaton::SIZE_CHUNK.y);
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
                        pos.x += cellautomaton::SIZE_CHUNK.x; // Wrap around to the last cell in the left chunk
                    }
                    else if (pos.x >= cellautomaton::SIZE_CHUNK.x)
                    {
                        targetChunk.x += 1; // Move to the right chunk
                        pos.x -= cellautomaton::SIZE_CHUNK.x; // Wrap around to the first cell in the right chunk
                    }

                    if (pos.y < 0)
                    {
                        targetChunk.y -= 1; // Move to the chunk above
                        pos.y += cellautomaton::SIZE_CHUNK.y; // Wrap around to the last cell in the above chunk
                    }
                    else if (pos.y >= cellautomaton::SIZE_CHUNK.y)
                    {
                        targetChunk.y += 1; // Move to the chunk below
                        pos.y -= cellautomaton::SIZE_CHUNK.y; // Wrap around to the first cell in the below chunk
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
        // FIXME instead of std::find, we can convert the chunk index from 2D to 1D dimension array
        auto found = std::find_if(m_chunks.begin(), m_chunks.end(), [pos](ChunkCellularAutomaton& cell) { return cell.getPosition() == pos; });
        if (found != m_chunks.end())
        {
            return &(*found);
        }
        return nullptr;
    }

}
