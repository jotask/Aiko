#pragma once

#include <vector>
#include <optional>
#include <array>

#include "aiko_types.h"
#include "shared/math.h"

#include "systems/render_system.h"
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

        std::vector<Color> getPixels();
        ivec2 getPosition() { return { x, y }; };

        std::vector<CellState> getNeighbours(ivec2 cell);

        std::optional<CellState> getCell(const ivec2 pos);

        texture::PboTexture getPbo();

    private:

        WorldCellularAutomaton* world = nullptr;
        const int x;
        const int y;

        std::array<CellState, cellautomaton::SIZE_CHUNK * cellautomaton::SIZE_CHUNK> cells;

        void refreshPixels(bool = false);
        void updatePixel(uint16_t x, uint16_t y, Color c);
        void updatePixels(std::vector<Color> pixels);

        texture::PboTexture m_texture;
        std::vector<Color> pixels;
        bool is_dirty = false;

        // Cells m_cells;

    };

}