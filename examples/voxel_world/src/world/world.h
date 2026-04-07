#pragma once


#include <unordered_map>

#include "chunk.h"
#include "assets/types/mesh_asset.h"
#include "models/material.h"
#include "models/mesh.h"
#include "types/world_types.h"

namespace aiko
{
    class IComponentAssetAccess;
    class RenderSystem;
}

namespace vw
{

    class World
    {
    public:
        void setup(aiko::RenderSystem*, aiko::IComponentAssetAccess*);
        void generate();
        void update();
        void render();
    private:

        struct ChunkData
        {
            Chunk chunk = {};
            aiko::Material material = {};
            aiko::AssetId meshId = aiko::InvalidAssetId;
        };

        aiko::RenderSystem* m_renderSystem;
        aiko::IComponentAssetAccess* m_assetsAccess;

        std::unordered_map<ChunkKey, ChunkData, ChunkKeyHash> m_chunks;

        void unload();

    };

}

