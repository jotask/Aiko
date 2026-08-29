#pragma once

#include "components/compute_shader_component.h"
#include "systems/base_system.h"

#include "types/compute_pass.h"

namespace aiko
{

    class RenderSystem;
    class SceneSystem;

    class ComputeSystem : public BaseSystem
    {
    public:

        ComputeSystem();
        virtual ~ComputeSystem() = default;

        const Texture* getOutputTexture(const ComputeShaderComponent* component) const;

    protected:

        virtual void update() override;
        virtual void render() override;
        virtual void dispose() override;

        virtual void connect(ModuleConnector*, SystemConnector*) override;

    private:

        struct RuntimeState
        {
            bool initialized = false;
            bool dispatched = false;
            bool readbackRequested = false;
            ReadbackId readbackId = InvalidReadbackId;
            ComputeBuffer buffer;

            // Texture
            Texture output;
            uint32_t outputWidth = 0;
            uint32_t outputHeight = 0;

            // Execution mode
            float accumulator = 0.0f;
            bool dirty = false;
        };

        struct ComputeParams
        {
            vec4 u_params = vec4(0.0f);
        };

        void updateComponent(GameObject*, ComputeShaderComponent&);
        void renderComponent(GameObject*, ComputeShaderComponent&);

        RuntimeState* tryGetState(const ComputeShaderComponent* cmp);
        const RuntimeState* tryGetState(const ComputeShaderComponent* cmp) const;

        RuntimeState& getOrCreateState(const ComputeShaderComponent*);
        void destroyStates();

        bool shouldDispatch(ComputeShaderComponent& cmp, RuntimeState& state);

        std::unordered_map<const ComputeShaderComponent*, AikoUPtr<RuntimeState>> m_runtime;
        ReadbackId m_nextReadbackId = InvalidReadbackId + 1;

        RenderSystem* m_renderSystem;
        SceneSystem* m_sceneSystem;


    };

}
