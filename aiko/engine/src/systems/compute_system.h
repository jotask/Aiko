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

    protected:

        virtual void update() override;
        virtual void dispose() override;

        virtual void connect(ModuleConnector*, SystemConnector*) override;

    private:

        struct RuntimeState
        {
            bool initialized = false;
            bool dispatched = false;
            bool readbackRequested = false;
            ReadbackId readbackId = 0;
            ComputeBuffer buffer;
        };

        void updateComponent(GameObject*, ComputeShaderComponent&);
        RuntimeState& getOrCreateState(const ComputeShaderComponent*);
        void destroyStates();

        std::unordered_map<const ComputeShaderComponent*, AikoUPtr<RuntimeState>> m_runtime;
        ReadbackId m_nextReadbackId = 1;

        RenderSystem* m_renderSystem;
        SceneSystem* m_sceneSystem;


    };

}
