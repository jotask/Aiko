#pragma once

#include <aiko_physics.h>

#include "systems/base_system.h"
#include "components/rigid_body_component.h"
#include "components/player_controller_component.h"

namespace aiko
{
    class RenderSystem;
    class AssetSystem;
    class SceneSystem;

    class PhysicsSystem : public BaseSystem
    {
    public:
        PhysicsSystem();
        ~PhysicsSystem() override = default;

        physics::AikoPhysics& getPhysics() { return m_physics; }

    protected:
        void connect(ModuleConnector*, SystemConnector*) override;
        void init() override;
        void update() override;
        void render() override;
        void dispose() override;

    private:
        MeshAsset makeMeshAssetFromPhysics(const physics::BodyMeshData& data);

        void ensureDebugMesh(RigidBodyComponent* component);
        void ensureDebugMesh(PlayerControllerComponent* component);

    private:
        RenderSystem* m_renderSystem = nullptr;
        AssetSystem* m_assetSystem = nullptr;

        physics::AikoPhysics m_physics;

        SceneSystem* m_sceneSystem = nullptr;

        float m_physicsAccumulator = 0.0f;
        Material m_debugMaterial;
    };
}