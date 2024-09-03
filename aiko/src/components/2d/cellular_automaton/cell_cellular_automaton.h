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

        CellCellularAutomaton(ChunkCellularAutomaton* chunk,const int i, const int j, const Color color);
        virtual ~CellCellularAutomaton() = default;

        void init();
        void update();

        Color getColor();
        void setColor(Color);

    private:

        const ChunkCellularAutomaton* chunk;
        const int i;
        const int j;

        Color color;

    };

}