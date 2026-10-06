#pragma once

#include <aiko_includes.h>

#include "cell_cellular_automaton_helper.h"
#include "cell_cellular_automaton.h"

namespace aiko
{
    class LayerContext;

    namespace ca
    {
        class WorldCellularAutomaton;
        class ChunkCellularAutomaton;

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
            aiko::LayerContext* m_context = nullptr;

            aiko::Mesh m_mesh;
            aiko::Material m_material;
        };

    }
}
