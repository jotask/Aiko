#pragma once

#include <unordered_map>

#include <aiko_types.h>
#include "systems/base_system.h"
#include "types/compute_pass.h"

namespace aiko
{
    class RenderModule;
    class AssetsManagerModule;
    class SceneSystem;
    class RenderSystem;
    class ParticleEmitterComponent;
    class AssetSystem;
    class GameObject;

    class ParticleSystem : public BaseSystem
    {
    public:
        ParticleSystem() = default;
        virtual ~ParticleSystem() override = default;
    protected:
        virtual void connect(ModuleConnector*, SystemConnector*) override;
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
        virtual void dispose() override;
    
    private:

        struct ParticleInitPushConstants
        {
            vec4 u_params;
            vec4 u_emitterPos;
        };

        struct ParticleUpdatePushConstants
        {
            alignas(16) vec3 emitterPosition;
            float lifetime;
            alignas(16) vec3 gravity;
            float startSpeed;
            alignas(16) vec3 direction;
            float directionRandomness;
            alignas(16) vec3 spawnBoxExtents;
            float spawnRadius;
            alignas(16) uint32_t spawnStart;
            uint32_t spawnCount;
            uint32_t particleCount;
            uint32_t spawnShape;
            alignas(16) uint32_t spawnSeed;
            uint32_t padding0 = 0;
            uint32_t padding1 = 0;
            uint32_t padding2 = 0;
        };

        static_assert(sizeof(ParticleUpdatePushConstants) == 96, "Particle update push constant ABI changed");
        static_assert(sizeof(ParticleUpdatePushConstants) <= MaxComputePushConstantBytes);

        struct RuntimeState
        {
            bool initialized = false;
            bool initDispatched = false;

            uint32_t spawnSeed = 0;

            float spawnAccumulator = 0.0f;
            uint32_t spawnThisFrame = 0;
            uint32_t spawnCursor = 0;

            ComputeBuffer positionBuffer;
            ComputeBuffer velocityBuffer;
            ComputeBuffer lifeBuffer;

            ComputeReadbackResult lastReadback;
            bool readbackRequested = false;
            ReadbackId readbackId = InvalidReadbackId;

            // GPU render
            bool renderInitialized = false;
            Mesh particleMesh;
            Material particleMaterial;
        };

        void updateEmitter(GameObject*, ParticleEmitterComponent&);
        void renderEmitter(GameObject*, ParticleEmitterComponent&);

        RuntimeState* tryGetState(const ParticleEmitterComponent* cmp);
        RuntimeState& getOrCreateState(const ParticleEmitterComponent* cmp);
        void destroyStates();

        RenderModule* m_renderModule;
        AssetsManagerModule* m_assetManagerModule;
        RenderSystem* m_renderSystem;
        SceneSystem* m_sceneSystem;
        AssetSystem* m_assetSystem;

        AssetId m_initShaderId = InvalidAssetId;
        AssetId m_updateShaderId = InvalidAssetId;

        ReadbackId m_nextReadbackId = InvalidReadbackId + 1;

        std::unordered_map<const ParticleEmitterComponent*, AikoUPtr<RuntimeState>> m_runtime;

        const bool m_debugReadbackEnabled = false;

    };

}
