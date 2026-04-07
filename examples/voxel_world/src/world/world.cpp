#include "world.h"

#include <systems/render_system.h>

#include "generator/chunk_data_generator.h"
#include "world/generator/chunk_mesh_generator.h"

#include <imgui.h>

#include "voxel_world_constants.h"

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
            chunk.material.m_baseColor = aiko::WHITE;
            chunk.material.m_useVertexColor = true;
            chunk.material.m_lit = true;

            ChunkDataGenerator::clearChunkData(chunk.chunk.getData());
            ChunkDataGenerator::generateChunkData(generationConfig, key.coord, chunk.chunk.getData());

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
      if (m_regenerationRequested == true)
      {
          m_regenerationRequested = false;
          generate();
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

    void World::gizmos()
    {
        if (c_imgui_world_generation)
        if (ImGui::Begin("ChunkGeneration"))
        {
            ImGui::Text("Generation");
            ImGui::DragInt("Seed", &generationConfig.seed);
            ImGui::SliderFloat("NoiseScale", &generationConfig.noiseScale, 0.0001f, 0.1f );
            ImGui::SliderInt("Octaves", &generationConfig.octaves, 0, 32);
            ImGui::SliderFloat("Persistance", &generationConfig.persistance, 0.0f, 1.0f);
            ImGui::SliderFloat("Amplitude", &generationConfig.amplitude, 0.0f, 100.0f);
            if (ImGui::Button("RegenerateWorld") == true)
            {
                m_regenerationRequested = true;
            }
            ImGui::End();
        }

        if (c_imgui_world_plot_test)
        if (ImGui::Begin("PlotGeneration"))
        {
            constexpr std::size_t amount = 100;
            const auto samples = ChunkDataGenerator::generatePlotTest(generationConfig, amount);
            ImGui::PlotLines("Samples", samples.data(), amount);
            ImGui::End();
        }
    }

    void World::unload()
    {
        // TODO unload meshes
        m_chunks.clear();
    }
}

