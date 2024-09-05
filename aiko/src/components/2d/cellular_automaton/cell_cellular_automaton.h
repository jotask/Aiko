#pragma once

#include "aiko_types.h"
#include "types/color.h"

#include "components/2d/cellular_automaton/cell_cellular_automaton_helper.h"

namespace aiko
{
    class ChunkCellularAutomaton;
    class CellCellularAutomaton
    {
    public:

        enum class CellState
        {
            LIVE,
            DEAD,
            DEBUG,
        };

        CellCellularAutomaton(ChunkCellularAutomaton* chunk,const int i, const int j);
        virtual ~CellCellularAutomaton() = default;

        void update();

        void setState(CellState);
        CellState getState();

        Color getColor();

        ChunkCellularAutomaton* getChunk();

        ivec2 getPosition() { return { i, j }; };

    private:

        ChunkCellularAutomaton* m_chunk = nullptr;
        const int i;
        const int j;
        CellState state;

    };

}