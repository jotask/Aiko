#pragma once

#include "aiko_types.h"
#include "shared/math.h"

#include <vector>
#include "cell_cellular_automaton.h"
#include "components/2d/cellular_automaton/cell_cellular_automaton_helper.h"

namespace aiko
{

    class WorldCellularAutomaton;
    class ChunkCellularAutomaton
    {
    public:

        using Cells = std::vector<CellCellularAutomaton>;

        ChunkCellularAutomaton(WorldCellularAutomaton* world, const int x, const int y);
        virtual ~ChunkCellularAutomaton() = default;

        void init();
        void update();
        void render();

        std::vector<Color> getPixels();
        Cells& getCells();
        ivec2 getPosition() { return { x, y }; };

    private:

        const WorldCellularAutomaton* world;
        const int x;
        const int y;

        Cells m_cells;

    };

}