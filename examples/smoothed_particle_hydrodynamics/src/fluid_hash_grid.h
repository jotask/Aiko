#pragma once

#include "aiko_types.h"
#include "sph_types.h"

#include <unordered_map>

namespace sph
{

    class FluidHashGrid
    {
    public:

        void init(const aiko::vector<SPHParticle>* particles, float cellSize);

        void clearGrid();

        void mapParticlesToCell();

        aiko::u64 getGridHashFromPosition(aiko::vec3 position) const;

        aiko::vector<size_t>* getContentOfCell(aiko::u64 hash);

        aiko::ivec3 getGridIdFromPosition(aiko::vec3 position) const;

        aiko::u64 cellIndexToHash(aiko::ivec3 position) const;

        aiko::vector<size_t> getNeighbourOfParticlesIdx(size_t index);

    private:

        float m_cellSize = 1.0f;

        std::unordered_map<aiko::u64, aiko::vector<size_t>> m_hashMap;

        const aiko::vector<SPHParticle>* m_particles = nullptr;
    };

}
