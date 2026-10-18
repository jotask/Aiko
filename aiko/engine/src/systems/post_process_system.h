
#pragma once

#include "systems/base_system.h"

namespace aiko
{
    class RenderModule;
    class AssetSystem;
    class PostProcessPipeline;

    class PostProcessSystem final : public BaseSystem
    {
    public:
        void init() override;

        PostProcessPipeline& pipeline() const;

    protected:
        void connect(ModuleConnector* modules, SystemConnector* systems) override;

    private:
        RenderModule* m_renderModule = nullptr;
        AssetSystem* m_assetSystem = nullptr;
    };
}
