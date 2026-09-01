#include "world.h"

#include <format>
#include <systems/render_system.h>
#include <application/application.h>

#include "generator/chunk_data_generator.h"
#include "models/game_object.h"
#include "world/generator/chunk_mesh_generator.h"
#include "layers/layer_context.h"

#include <imgui.h>

#include "voxel_world_constants.h"
#include "bridge/mesh_asset_to_shape.h"
#include "systems/physics_system.h"

namespace vw
{

    void World::setup(aiko::LayerContext& context)
    {
        m_context = &context;
        m_worldRoot = m_context->Instantiate("World");
        AIKO_ASSERT(m_worldRoot != nullptr, "Couldn't create World Root GameObject")
    }

    void World::generate()
    {

        unload();

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
            aiko::Transform& trans = chunk.second.m_object->transform();
            trans.position = {chunk.first.coord.x * static_cast<float>(CHUNK_SIZE.x), 0.0f, chunk.first.coord.y * static_cast<float>(CHUNK_SIZE.z) };
            trans.rotation = {0.0f};
            trans.scale = {1.0f};
        }
        if (m_regenerationRequested == true)
        {
          m_regenerationRequested = false;
          generate();
        }
    }

    void World::gizmos()
    {
        if (c_imgui_world_generation)
        {
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
        }

        if (c_imgui_world_plot_test)
        {
            if (ImGui::Begin("PlotGeneration"))
            {
                constexpr std::size_t amount = 100;
                const auto samples = ChunkDataGenerator::generatePlotTest(generationConfig, amount);
                ImGui::PlotLines("Samples", samples.data(), amount);
                ImGui::End();
            }
        }
    }

    void World::generateChunk(int x, int z)
    {

        const ChunkCoord coord = { x, z };

        ChunkData data = {};
        ChunkDataGenerator::clearChunkData(data);
        ChunkDataGenerator::generateChunkData(generationConfig, coord, data);

        aiko::MeshAsset asset = ChunkMeshGenerator::generateMeshAsset(data);

        aiko::physics::TriangleMeshShapeDesc shape = aiko::physics::makeTriangleMeshShapeDesc(asset);

        Chunk chunk = {};

        const std::string chunk_name = std::format("Chunk({}, {})", x, z);
        aiko::GameObject* chunkGO = m_context->Instantiate(m_worldRoot, chunk_name.c_str());

        auto meshCMP = chunkGO->addComponent<aiko::MeshComponent>();
        auto bodyCMP = chunkGO->addComponent<aiko::RigidBodyComponent>();

        meshCMP->load(asset);

        aiko::Transform transform;
        transform.position =
        {
            coord.x * static_cast<float>(CHUNK_SIZE.x),
            0.0f,
            coord.y * static_cast<float>(CHUNK_SIZE.z)
        };
        transform.rotation = { 0.0f, 0.0f, 0.0f };
        transform.scale = { 1.0f, 1.0f, 1.0f };

        aiko::physics::BodyDesc desc = {};
        desc.motionType = aiko::physics::MotionType::Static;
        desc.transform = transform;
        desc.shape.type = aiko::physics::ShapeType::TriangleMesh;
        desc.shape.triangleMesh = std::move(shape);
        bodyCMP->create(desc);

        const ChunkKey key = { .coord = { x, z} };
        const ChunkObj chunkObj =
        {
            .chunk =  {},
            .m_object = chunkGO,
            .m_mesh = meshCMP,
            .m_body = bodyCMP,
        };

        m_chunks.emplace(key, chunkObj);

    }

    void World::unload()
    {
        // TODO remove game objects
        m_chunks.clear();
    }
}

