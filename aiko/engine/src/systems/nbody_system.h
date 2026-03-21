#pragma once

#include <unordered_map>

#include <aiko_types.h>
#include "systems/base_system.h"
#include "types/compute_pass.h"

namespace aiko
{
    class NBodyComponent;
    class RenderModule;
    class AssetsManagerModule;
    class SceneSystem;
    class RenderSystem;
    class GameObject;

    class NBodySystem : public BaseSystem
    {
    public:
        NBodySystem() = default;
        virtual ~NBodySystem() override = default;
    protected:
        virtual void connect(ModuleConnector*, SystemConnector*) override;
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
        virtual void dispose() override;
    
    private:

        struct RuntimeState
        {
            bool initialized = false;
            bool initDispatched = false;

            ComputeBuffer positionMassBuffer;
            ComputeBuffer velocityBuffer;

            ComputeBuffer positionMassBufferNext;
            ComputeBuffer velocityBufferNext;

            ComputeBuffer* positionMassCurrent = nullptr;
            ComputeBuffer* velocityCurrent = nullptr;

            ComputeBuffer* positionMassWrite = nullptr;
            ComputeBuffer* velocityWrite = nullptr;

            bool renderInitialized = false;
            Material bodyMaterial;
        };

        void updateSimulation(GameObject*, NBodyComponent&);
        void renderSimulation(GameObject*, NBodyComponent&);

        RuntimeState* tryGetState(const NBodyComponent* cmp);
        RuntimeState& getOrCreateState(const NBodyComponent* cmp);
        void destroyStates();

        RenderModule* m_renderModule;
        AssetsManagerModule* m_assetManagerModule;
        RenderSystem* m_renderSystem;
        SceneSystem* m_sceneSystem;

        AssetId m_initShaderId = InvalidAssetId;
        AssetId m_updateShaderId = InvalidAssetId;

        ReadbackId m_nextReadbackId = InvalidReadbackId + 1;

        std::unordered_map<const NBodyComponent*, AikoUPtr<RuntimeState>> m_runtime;

    };

}
