#pragma once

#include <vector>
#include <array>

#include "aiko_types.h"
#include "shared/math.h"

#include "cell_cellular_automaton_helper.h"

namespace aiko::ca
{

    class WorldCellularAutomaton;
    class ChunkCellularAutomaton
    {
    public:

        enum class CellState
        {
            NULLPTR,
            LIVE,
            DEAD,
            DEBUG,
        };

        ChunkCellularAutomaton(WorldCellularAutomaton* world, const ivec2 pos);
        virtual ~ChunkCellularAutomaton() = default;

        void init();
        void update();

        ivec2 getPosition() { return pos; };

        std::vector<CellState> getNeighbours(ivec2 cell);

        CellState getCell(const ivec2 pos);

        Color getColorFromCell(CellState);

    private:

        WorldCellularAutomaton* world = nullptr;
        const ivec2 pos;

        std::array<CellState, cellautomaton::SIZE_CHUNK * cellautomaton::SIZE_CHUNK> prev_cells;
        std::array<CellState, cellautomaton::SIZE_CHUNK * cellautomaton::SIZE_CHUNK> cells;

    };

}