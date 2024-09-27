#pragma once

#include "cell_cellular_automaton_helper.h"
#include "cell_cellular_automaton.h"

namespace aiko
{
    class RenderSystem;
    namespace ca
    {
        class WorldCellularAutomaton;
        class ChunkCellularAutomaton;
        namespace cellautomaton
        {

            class AutomatonRender
            {
            private:
                const bool s_render_cells = true;
                RenderSystem* m_renderSystem;

            public:

                AutomatonRender();
                ~AutomatonRender() = default;

                void init(RenderSystem* m_renderSystem);

                void render(WorldCellularAutomaton* world);

                void drawChunk(ChunkCellularAutomaton* chunk);

                Color getColorFromCell(CellCellularAutomaton::CellState stat);

            };

        }
    }
}
