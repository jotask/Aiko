#pragma once

#include <vector>
#include <array>

#include "aiko_types.h"
#include "shared/math.h"

#include "cell_cellular_automaton_helper.h"
#include "cell_cellular_automaton.h"

namespace aiko::ca
{

    class WorldCellularAutomaton;
    class ChunkCellularAutomaton
    {
    public:

        ChunkCellularAutomaton(WorldCellularAutomaton* world, const ivec2 pos);
        virtual ~ChunkCellularAutomaton() = default;

        void init();
        void update();

        ivec2 getPosition() { return pos; };

        CellCellularAutomaton* getCell(const ivec2);

        WorldCellularAutomaton* getWorld();

    private:

        WorldCellularAutomaton* world = nullptr;
        const ivec2 pos;

        std::vector<CellCellularAutomaton> cells;

    };

}