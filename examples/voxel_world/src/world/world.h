#pragma once

#include "aiko_physics.h"
#include "assets/types/mesh_asset.h"
#include "chunk.h"
#include "components/mesh_component.h"
#include "components/rigid_body_component.h"
#include "generator/chunk_data_generator.h"
#include "models/material.h"
#include "types/world_types.h"

#include <unordered_map>

namespace aiko
{
    class LayerContext;
    class PhysicsSystem;
    class IComponentAssetAccess;
    class RenderSystem;
}

namespace vw
{

    class World
    {
    public:
        void setup(aiko::LayerContext& context);
        void generate();
        void update();
        void gizmos();
    private:

        struct ChunkObj
        {
            Chunk chunk = {};
            aiko::GameObject* m_object;
            aiko::MeshComponent* m_mesh;
            aiko::RigidBodyComponent* m_body;
        };

        aiko::LayerContext* m_context = nullptr;
        aiko::GameObject* m_worldRoot = nullptr;

        ChunkDataGenerator::GeneratorSettings generationConfig = {};

        std::unordered_map<ChunkKey, ChunkObj, ChunkKeyHash> m_chunks;

        bool m_regenerationRequested = false;

        void generateChunk(int x, int z);
        void unload();

    };

}

