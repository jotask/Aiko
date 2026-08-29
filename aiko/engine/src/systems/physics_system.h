#pragma once

#include <aiko_physics.h>

#include "systems/base_system.h"
#include "components/rigid_body_component.h"
#include "components/player_controller_component.h"

namespace aiko
{
    class RenderSystem;
    class AssetSystem;

    class PhysicsSystem : public BaseSystem
    {
    public:
        PhysicsSystem();
        ~PhysicsSystem() override = default;

        void registerRigidBody(RigidBodyComponent* component);
        void unregisterRigidBody(RigidBodyComponent* component);

        void registerPlayerController(PlayerControllerComponent* component);
        void unregisterPlayerController(PlayerControllerComponent* component);

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

        vector<RigidBodyComponent*> m_rigidBodies;
        vector<PlayerControllerComponent*> m_playerControllers;

        float m_physicsAccumulator = 0.0f;
        Material m_debugMaterial;
    };
}