
#include "post_process_system.h"

#include "modules/render_module.h"
#include "modules/module_connector.h"

#include "systems/asset_system.h"
#include "systems/system_connector.h"

#include <post_process/post_process_pipeline.h>
#include <post_process/tone_mapping_effect.h>

#include <assets/types/shader_asset.h>

namespace aiko
{
    void PostProcessSystem::connect(ModuleConnector* modules, SystemConnector* systems)
    {
        BIND_MODULE_REQUIRED(RenderModule, modules, m_renderModule);
        BIND_SYSTEM_REQUIRED(AssetSystem, systems, m_assetSystem);
    }

    PostProcessPipeline& PostProcessSystem::pipeline() const
    {
        AIKO_ASSERT(m_renderModule != nullptr, "RenderModule missing");
        return m_renderModule->postProcessing();
    }


    void PostProcessSystem::init()
    {
        // Internal display conversion, independent of the effect list.
        const AssetId displayShader = m_assetSystem->registerAsset<ShaderAsset>("post_process/post_display");

        m_assetSystem->loadAsset<ShaderAsset>(displayShader);
        pipeline().setDisplayConversionShader(displayShader);

        // Aiko's default, user-configurable tone-mapping effect.
        const AssetId toneMappingShader = m_assetSystem->registerAsset<ShaderAsset>("post_process/post_tonemap");

        m_assetSystem->loadAsset<ShaderAsset>(toneMappingShader);

        auto& toneMapping = pipeline().addEffect<ToneMappingEffect>(toneMappingShader);

        toneMapping.setExposure(1.0f);
    }

}
