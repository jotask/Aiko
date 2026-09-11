#pragma once

#include "types/chunk_types.h"

namespace vw
{

    class Chunk
    {
    public:
        Chunk();
        void update();
        void render();
        const ChunkData& getData() const { return m_voxels; }
        ChunkData& getData() { return m_voxels; }
    private:

        ChunkData m_voxels;

    };

}

