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

        ChunkCellularAutomaton(WorldCellularAutomaton* world, const int x, const int y);
        virtual ~ChunkCellularAutomaton() = default;

        void init();
        void update();
        void render();

        ivec2 getPosition() { return { x, y }; };

        std::vector<CellState> getNeighbours(ivec2 cell);

        std::optional<CellState> getCell(const ivec2 pos);

    private:

        Color getColorFromCell(CellState);

        WorldCellularAutomaton* world = nullptr;
        const int x;
        const int y;

        std::array<CellState, cellautomaton::SIZE_CHUNK * cellautomaton::SIZE_CHUNK> cells;

    };

}