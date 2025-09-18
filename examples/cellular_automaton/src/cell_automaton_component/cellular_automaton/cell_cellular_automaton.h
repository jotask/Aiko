#pragma once

#include <vector>
#include <array>

#include "aiko_types.h"
#include "shared/math.h"

#include "cell_cellular_automaton_helper.h"

namespace aiko::ca
{

    class WorldCellularAutomaton;
    class ChunkCellularAutomaton;
    class CellCellularAutomaton
    {
    public:

        enum class CellState
        {
            NULLPTR,
            LIVE,
            DEAD,
            DEBUG,
        };

        CellCellularAutomaton(ChunkCellularAutomaton* chunk, const ivec2 pos, CellState cellstate = CellState::DEAD);
        virtual ~CellCellularAutomaton() = default;

        void init();
        void preUpdate();
        void update();

        void updateNeighbours();

        ivec2 getPosition() { return pos; };

        void setState(CellState);
        CellState getState();
        CellState getPrevState();

        std::vector<CellCellularAutomaton*> getCache();
        ChunkCellularAutomaton* getChunk();

    private:

        ChunkCellularAutomaton* chunk = nullptr;
        const ivec2 pos;

        std::vector<CellCellularAutomaton*> cache_neighbours;

        CellState prev_state;
        CellState state;
        CellState next_state;

    };

}