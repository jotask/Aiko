#include "fluid_hash_grid.h"

#include "simulation.h"

#include <cmath>

namespace sph
{

    FluidHashGrid::FluidHashGrid() = default;
    FluidHashGrid::~FluidHashGrid() = default;

    void FluidHashGrid::init(Simulation* simulation, float cellSize)
    {
        AIKO_ASSERT(simulation != nullptr, "FluidHashGrid requires Simulation");
        m_simulation = simulation;
        AIKO_ASSERT(cellSize > 0.0f, "FluidHashGrid cell size must be positive");
        m_cellSize = cellSize;
    }

    void FluidHashGrid::clearGrid()
    {
        m_hashMap = {};
    }

    aiko::ivec3 FluidHashGrid::getGridIdFromPosition(const aiko::vec3 position)
    {
        const int x = static_cast<int>(std::floor(position.x / m_cellSize));
        const int y = static_cast<int>(std::floor(position.y / m_cellSize));
        return {x, y, 0};
    }

    aiko::u64 FluidHashGrid::getGridHashFromPosition(const aiko::vec3 position)
    {
        const aiko::ivec3 gridID = getGridIdFromPosition(position);
        return cellIndexToHash(gridID);
    }

    aiko::u64 FluidHashGrid::cellIndexToHash(const aiko::ivec3 position)
    {
        const aiko::u64 x = static_cast<aiko::u32>(position.x);
        const aiko::u64 y = static_cast<aiko::u32>(position.y);
        return (x << 32) | y;
    }

    void FluidHashGrid::mapParticlesToCell()
    {
        const auto& particles = m_simulation->particles();

        for (size_t i = 0; i < particles.size(); ++i)
        {
            const SPHParticle& particle = particles[i];
            const aiko::u64 hash = getGridHashFromPosition(particle.position);
            m_hashMap[hash].push_back(i);
        }
    }

    aiko::vector<size_t>* FluidHashGrid::getContentOfCell(aiko::u64 hash)
    {
        const auto it = m_hashMap.find(hash);
        if (it == m_hashMap.end())
        {
            return nullptr;
        }
        return &it->second;
    }

}
