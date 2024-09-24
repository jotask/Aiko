#pragma once

#include <vector>

#include "aiko_types.h"
#include "chunk_cellular_automaton.h"
#include "cell_cellular_automaton_helper.h"

namespace aiko
{
    class RenderSystem;
    namespace ca
    {
        class WorldCellularAutomaton
        {
        public:

            using Chunks = std::vector<ChunkCellularAutomaton>;

            WorldCellularAutomaton();
            virtual ~WorldCellularAutomaton() = default;

            void init();
            void update();

            Chunks& getChunks();

            std::vector<ChunkCellularAutomaton::CellState> getNeighbours(ivec2 chunk, ivec2 cell);

            ChunkCellularAutomaton* getChunk(const ivec2 pos);

        private:

            Chunks m_chunks;

        };

    }
}
