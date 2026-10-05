#include "fluid_hash_grid.h"

#include "intrumentor/profiler.h"

#include <cmath>

namespace sph
{

    void FluidHashGrid::init(const aiko::vector<SPHParticle>* particles, float cellSize)
    {
        AIKO_ASSERT(particles != nullptr, "FluidHashGrid requires particle storage");
        AIKO_ASSERT(cellSize > 0.0f, "FluidHashGrid cell size must be positive");
        m_particles = particles;
        m_cellSize = cellSize;
    }

    void FluidHashGrid::clearGrid()
    {
        AIKO_FUNCTION_PROFILE

        m_hashMap.clear();
    }

    aiko::ivec3 FluidHashGrid::getGridIdFromPosition(aiko::vec3 position) const
    {
        const int x = static_cast<int>(std::floor(position.x / m_cellSize));
        const int y = static_cast<int>(std::floor(position.y / m_cellSize));
        return{ x, y, 0 };
    }

    aiko::u64 FluidHashGrid::getGridHashFromPosition(aiko::vec3 position) const
    {
        return cellIndexToHash(getGridIdFromPosition(position));
    }

    aiko::u64 FluidHashGrid::cellIndexToHash(aiko::ivec3 position) const
    {
        const aiko::u64 x = static_cast<aiko::u32>(position.x);
        const aiko::u64 y = static_cast<aiko::u32>(position.y);
        return (x << 32) | y;
    }

    aiko::vector<size_t> FluidHashGrid::getNeighbourOfParticlesIdx(size_t index)
    {
        AIKO_ASSERT(m_particles != nullptr, "FluidHashGrid is not initialized");
        AIKO_ASSERT(index < m_particles->size(), "Particle index out of range");

        aiko::vector<size_t> neighbours;

        const aiko::vec3 position = (*m_particles)[index].position;

        const aiko::ivec3 gridId = getGridIdFromPosition(position);

        constexpr int NeighbourRadius = 1;

        for (int y = -NeighbourRadius; y <= NeighbourRadius; ++y)
        {
            for (int x = -NeighbourRadius; x <= NeighbourRadius; ++x)
            {
                const aiko::ivec3 grid =
                {
                    gridId.x + x,
                    gridId.y + y,
                    0
                };

                const aiko::u64 hash = cellIndexToHash(grid);

                aiko::vector<size_t>* content = getContentOfCell(hash);

                if (content == nullptr)
                {
                    continue;
                }

                neighbours.insert(neighbours.end(), content->begin(), content->end());
            }
        }

        return neighbours;
    }

    void FluidHashGrid::mapParticlesToCell()
    {
        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(m_particles != nullptr, "FluidHashGrid is not initialized");

        for (size_t i = 0; i < m_particles->size(); ++i)
        {
            const SPHParticle& particle = (*m_particles)[i];

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
