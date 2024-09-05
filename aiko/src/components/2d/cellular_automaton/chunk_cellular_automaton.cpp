#include "chunk_cellular_automaton.h"

#include <assert.h>
#include <chrono>

#include "models/game_object.h"
#include "systems/render_system.h"
#include "constants.h"

#include "world_cellular_automaton.h"

#include "core/log.h"

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

        m_texture.texture.width = cellautomaton::SIZE_CHUNK;
        m_texture.texture.height = cellautomaton::SIZE_CHUNK;
        m_texture = world->getRenderSystem()->createPboTexture(m_texture.texture.width, m_texture.texture.height);
        pixels.reserve(m_texture.texture.width * m_texture.texture.height);
        pixels.resize(m_texture.texture.width * m_texture.texture.height);
        std::fill(pixels.begin(), pixels.end(), BLACK);
        is_dirty = true;

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

    }

    void ChunkCellularAutomaton::update()
    {
        for (int y = 0; y < cellautomaton::SIZE_CHUNK; y++)
        {
            for (int x = 0; x < cellautomaton::SIZE_CHUNK; x++)
            {

                const auto idx = cellautomaton::getIndex(x, y, cellautomaton::SIZE_CHUNK);
                auto state = cells[idx];
                
                std::vector<CellState> neighbours = getNeighbours({ x, y });

                const auto alive = std::count_if(neighbours.begin(), neighbours.end(), [](CellState& c)
                {
                    return c == CellState::LIVE;
                });

                if (state == ChunkCellularAutomaton::CellState::LIVE)
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
        }
        refreshPixels();
    }

    void ChunkCellularAutomaton::render()
    {
        for (auto& cell : cells)
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

                Color c;
                switch (cells[idx])
                {
                case CellState::LIVE:  c = WHITE;   break;
                case CellState::DEAD:  c = BLACK;   break;
                case CellState::DEBUG: c = MAGENTA; break;
                default:
                    break;
                }
                colors.push_back(c);
            }
        }
        return colors;
    }

    std::vector<ChunkCellularAutomaton::CellState> ChunkCellularAutomaton::getNeighbours(ivec2 cell)
    {
        return world->getNeighbours({x, y}, cell);
    }

    std::optional<ChunkCellularAutomaton::CellState> ChunkCellularAutomaton::getCell(const ivec2 pos)
    {
        const auto idx = cellautomaton::getIndex(pos.x, pos.y, cellautomaton::SIZE_CHUNK);
        if (idx >= 0 && idx < cellautomaton::SIZE_CHUNK)
        {
            return cells[idx];
        }
        return  std::nullopt;

    }

    texture::PboTexture ChunkCellularAutomaton::getPbo()
    {
        return m_texture;
    }

    void ChunkCellularAutomaton::refreshPixels(bool force)
    {
        if (force == true || is_dirty == true)
        {
            is_dirty = false;
            world->getRenderSystem()->updatePbo(m_texture, pixels);
        }
    }

    void ChunkCellularAutomaton::updatePixel(uint16_t x, uint16_t y, Color c)
    {
        const uint16_t index = y * m_texture.texture.width + x;
        if (pixels[index] == c)
        {
            return;
        }
        pixels[index] = c;
        is_dirty = true;
    }

    void ChunkCellularAutomaton::updatePixels(std::vector<Color> ps)
    {
        assert(pixels.size() == ps.size(), "New pixels don't match texture size");
        pixels.clear();
        pixels.insert(pixels.end(), ps.begin(), ps.end());
        is_dirty = true;
    }

}
