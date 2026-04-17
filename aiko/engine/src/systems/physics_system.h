#pragma once

#include <aiko_physics.h>

#include "systems/base_system.h"

namespace aiko
{

    class RenderSystem;

    class PhysicsSystem : public BaseSystem
    {
    public:
        PhysicsSystem();
        virtual ~PhysicsSystem() = default;

        // FIXME proxy for now
        physics::AikoPhysics& getPhysics() { return m_physics; }

    protected:

        virtual void connect(ModuleConnector*, SystemConnector*) override;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
        virtual void dispose() override;

    private:

        struct DebugBody
        {
            Transform transform{};
            physics::BodyId body = physics::InvalidBodyId;
            Mesh mesh = {};
        };

        vector<DebugBody> bodies;
        void generatedDebugPhysicsScene();

        MeshAsset makeMeshAssetFromPhysics(const physics::BodyMeshData& data);

        RenderSystem* m_renderSystem;
        physics::AikoPhysics m_physics;

        float m_physicsAccumulator = 0.0f;

        Material m_debugMaterial;

    };

}
