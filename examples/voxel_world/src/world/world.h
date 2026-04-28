#pragma once


#include <unordered_map>

#include "aiko_physics.h"
#include "chunk.h"
#include "assets/types/mesh_asset.h"
#include "components/mesh_component.h"
#include "components/rigid_body_component.h"
#include "generator/chunk_data_generator.h"
#include "models/material.h"
#include "types/world_types.h"

namespace aiko
{
    class Application;
    class PhysicsSystem;
    class IComponentAssetAccess;
    class RenderSystem;
}

namespace vw
{

    class World
    {
    public:
        void setup(aiko::Application* app);
        void generate();
        void update();
        void gizmos();
    private:

        struct ChunkObj
        {
            Chunk chunk = {};
            aiko::GameObject* m_object;
            aiko::AikoPtr<aiko::MeshComponent> m_mesh;
            aiko::AikoPtr<aiko::RigidBodyComponent> m_body;
        };

        aiko::Application* m_app;
        aiko::GameObject* m_worldRoot;

        ChunkDataGenerator::GeneratorSettings generationConfig = {};

        std::unordered_map<ChunkKey, ChunkObj, ChunkKeyHash> m_chunks;

        bool m_regenerationRequested = false;

        void unload();

    };

}

