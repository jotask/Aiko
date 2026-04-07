#pragma once


#include <unordered_map>

#include "chunk.h"
#include "assets/types/mesh_asset.h"
#include "generator/chunk_data_generator.h"
#include "models/material.h"
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
        void gizmos();
    private:

        struct ChunkData
        {
            Chunk chunk = {};
            aiko::Material material = {};
            aiko::AssetId meshId = aiko::InvalidAssetId;
        };

        ChunkDataGenerator::GeneratorSettings generationConfig = {};

        aiko::RenderSystem* m_renderSystem;
        aiko::IComponentAssetAccess* m_assetsAccess;

        std::unordered_map<ChunkKey, ChunkData, ChunkKeyHash> m_chunks;

        void unload();

    };

}

