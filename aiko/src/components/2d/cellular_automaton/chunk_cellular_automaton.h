#pragma once

#include <vector>
#include <optional>
#include <array>

#include "aiko_types.h"
#include "shared/math.h"

#include "components/2d/cellular_automaton/cell_cellular_automaton_helper.h"

namespace aiko
{

    class WorldCellularAutomaton;
    class ChunkCellularAutomaton
    {
    public:

        enum class CellState
        {
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

        std::optional<CellState> getCell(const ivec2 pos);

        Color getColorFromCell(CellState);

    private:

        WorldCellularAutomaton* world = nullptr;
        const ivec2 pos;

        std::array<CellState, cellautomaton::SIZE_CHUNK* cellautomaton::SIZE_CHUNK> prev_cells;
        std::array<CellState, cellautomaton::SIZE_CHUNK* cellautomaton::SIZE_CHUNK> cells;

    };

}