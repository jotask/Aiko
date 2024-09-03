#pragma once

#include <vector>

#include "aiko_types.h"
#include "components/2d/cellular_automaton/chunk_cellular_automaton.h"
#include "components/2d/cellular_automaton/cell_cellular_automaton_helper.h"

namespace aiko
{

    class WorldCellularAutomaton
    {
    public:

        using Chunks = std::vector<ChunkCellularAutomaton>;

        WorldCellularAutomaton();
        virtual ~WorldCellularAutomaton() = default;

        void init();
        void update();
        void render();

        Chunks getChunks();

    private:

        Chunks m_chunks;

    };

}