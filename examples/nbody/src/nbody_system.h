#pragma once

#include <aiko_types.h>
#include <systems/base_system.h>
#include <models/compute_buffer.h>
#include <models/material.h>
#include <types/compute_pass.h>

#include <unordered_map>

namespace aiko
{
    class RenderModule;
    class AssetsManagerModule;
    class SceneSystem;
    class RenderSystem;
    class AssetSystem;
    class GameObject;
}

namespace nbody
{

    class NBodyComponent;

    class NBodySystem : public aiko::BaseSystem
    {
    public:
        NBodySystem() = default;
        virtual ~NBodySystem() override = default;
    protected:
        virtual void connect(aiko::ModuleConnector*, aiko::SystemConnector*) override;
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
        virtual void dispose() override;

    private:

        struct RuntimeState
        {
            bool initialized = false;
            bool initDispatched = false;

            aiko::ComputeBuffer positionMassBuffer;
            aiko::ComputeBuffer velocityBuffer;

            aiko::ComputeBuffer indexBuffer;

            aiko::ComputeBuffer positionMassBufferNext;
            aiko::ComputeBuffer velocityBufferNext;

            aiko::ComputeBuffer* positionMassCurrent = nullptr;
            aiko::ComputeBuffer* velocityCurrent = nullptr;

            aiko::ComputeBuffer* positionMassWrite = nullptr;
            aiko::ComputeBuffer* velocityWrite = nullptr;

            aiko::ComputeBuffer indirectBuffer;

            bool renderInitialized = false;
            aiko::Material bodyMaterial;
        };

        void updateSimulation(aiko::GameObject*, NBodyComponent&);
        void renderSimulation(aiko::GameObject*, NBodyComponent&);

        RuntimeState* tryGetState(const NBodyComponent* cmp);
        RuntimeState& getOrCreateState(const NBodyComponent* cmp);
        void destroyStates();

        aiko::RenderModule* m_renderModule;
        aiko::AssetsManagerModule* m_assetManagerModule;
        aiko::RenderSystem* m_renderSystem;
        aiko::SceneSystem* m_sceneSystem;
        aiko::AssetSystem* m_assetSystem;

        aiko::AssetId m_initShaderId = aiko::InvalidAssetId;
        aiko::AssetId m_updateShaderId = aiko::InvalidAssetId;

        aiko::ReadbackId m_nextReadbackId = aiko::InvalidReadbackId + 1;

        std::unordered_map<const NBodyComponent*, aiko::AikoUPtr<RuntimeState>> m_runtime;

    };

}
