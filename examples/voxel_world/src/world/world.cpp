#include "world.h"

#include <systems/render_system.h>

#include "generator/chunk_data_generator.h"
#include "world/generator/chunk_mesh_generator.h"
#include "models/mesh_factory.h"

namespace vw
{

    void World::setup(aiko::RenderSystem* renderer, aiko::IComponentAssetAccess* assets)
    {
        m_renderSystem = renderer;
        m_assetsAccess = assets;

        AIKO_ASSERT(m_renderSystem != nullptr, "Render System null!")
        AIKO_ASSERT(m_assetsAccess != nullptr, "Assets Accessor null!")

    }

    void World::generate()
    {
        unload();

        auto generateChunk = [&](int x, int z)
        {
            const ChunkKey key = { .coord = { x, z} };
            m_chunks.emplace(key, Chunk());
            ChunkData& chunk = m_chunks.at(key);

            chunk.material.m_shaderId = m_assetsAccess->registerShader("model");
            chunk.material.m_baseColor = aiko::Color::getRandomColor();
            chunk.material.m_useVertexColor = true;
            chunk.material.m_lit = true;

            ChunkDataGenerator::clearChunkData(chunk.chunk.getData());
            ChunkDataGenerator::generateChunkData(key.coord, chunk.chunk.getData());

            chunk.meshId = m_assetsAccess->registerMesh(ChunkMeshGenerator::generateMeshAsset(chunk.chunk.getData()));
        };

        if constexpr (WORLD_SIZE == 1)
        {
            generateChunk(0, 0);
        }
        else
        {

            constexpr int HALF = static_cast<int>(WORLD_SIZE) / 2;
            for (int y = -HALF ; y < HALF ; ++y)
            {
                for (int x = -HALF ; x < HALF ; ++x)
                {
                    generateChunk(x, y);
                }
            }
        }
    }

    void World::update()
    {
        for (auto& chunk : m_chunks)
        {

        }
    }

    void World::render()
    {
        for (auto& chunk : m_chunks)
        {
            const aiko::Transform trans =
            {
                .position = {chunk.first.coord.x * static_cast<float>(CHUNK_SIZE.x), 0.0f, chunk.first.coord.y * static_cast<float>(CHUNK_SIZE.z) },
                .rotation = {0.0f},
                .scale = {1.0f}
            };
            m_renderSystem->submitMesh(trans, chunk.second.meshId, chunk.second.material );
        }
    }

    void World::unload()
    {
        // TODO unload meshes
        m_chunks.clear();
    }
}

