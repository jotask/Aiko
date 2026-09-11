#pragma once

#include "cell_cellular_automaton_helper.h"
#include "cell_cellular_automaton.h"

namespace aiko
{
    class LayerContext;
    namespace ca
    {
        class WorldCellularAutomaton;
        class ChunkCellularAutomaton;
        namespace cellautomaton
        {

            class AutomatonRender
            {
            public:

                AutomatonRender();
                ~AutomatonRender() = default;

                void init(aiko::LayerContext& context);

                void render(WorldCellularAutomaton* world);

                void drawChunk(ChunkCellularAutomaton* chunk);

                Color getColorFromCell(CellCellularAutomaton::CellState stat);

            private:
                const bool s_render_cells = true;
                aiko::LayerContext* m_context = nullptr;
            };

        }
    }
}
