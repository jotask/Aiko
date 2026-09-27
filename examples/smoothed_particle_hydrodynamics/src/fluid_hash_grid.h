#pragma once

#include "aiko_types.h"
#include "sph_types.h"

#include <unordered_map>

namespace sph
{

    class Simulation;

    class FluidHashGrid
    {
    public:

        FluidHashGrid();
        ~FluidHashGrid();

        void init(Simulation* simulation, float cellSize);

        void clearGrid();

        void mapParticlesToCell();
        aiko::u64 getGridHashFromPosition(const aiko::vec3 position);
        aiko::vector<size_t>* getContentOfCell(aiko::u64);
        aiko::ivec3 getGridIdFromPosition(const aiko::vec3 position);
        aiko::u64 cellIndexToHash(const aiko::ivec3 position);

    private:

        float m_cellSize = 1.0f;

        std::unordered_map<aiko::u64, aiko::vector<size_t>> m_hashMap;

        Simulation* m_simulation = nullptr;

    };


}
