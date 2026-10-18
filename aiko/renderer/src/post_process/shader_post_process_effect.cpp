
#include "shader_post_process_effect.h"

#include "post_process_context.h"

namespace aiko
{
    ShaderPostProcessEffect::ShaderPostProcessEffect(std::string name, AssetId shaderId, PostProcessDomain inputDomain, PostProcessDomain outputDomain)
        : PostProcessEffect(std::move(name), false)
        , m_inputDomain(inputDomain)
        , m_outputDomain(outputDomain)
    {
        AIKO_ASSERT(shaderId != InvalidAssetId, "Shader post-process effect requires a valid shader");
        AIKO_ASSERT(!(inputDomain == PostProcessDomain::LDR && outputDomain == PostProcessDomain::HDR), "LDR-to-HDR effects are not supported");

        m_material.m_shaderId = shaderId;
        m_material.m_lit = false;
        m_material.m_useVertexColor = false;

        m_material.m_renderState.depthTest = false;
        m_material.m_renderState.depthWrite = false;
        m_material.m_renderState.blend = false;
        m_material.m_renderState.cullMode = CullMode::None;
    }

    PostProcessDomain ShaderPostProcessEffect::inputDomain() const
    {
        return m_inputDomain;
    }

    PostProcessDomain ShaderPostProcessEffect::outputDomain() const
    {
        return m_outputDomain;
    }

    void ShaderPostProcessEffect::execute(PostProcessContext& context)
    {
        context.renderFullscreen(m_material);
    }

    Material& ShaderPostProcessEffect::material()
    {
        return m_material;
    }

    const Material& ShaderPostProcessEffect::material() const
    {
        return m_material;
    }
}
