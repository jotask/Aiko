
#include "tone_mapping_effect.h"

#include <cmath>

#include "post_process_context.h"

namespace aiko
{
    ToneMappingEffect::ToneMappingEffect(AssetId shaderId)
        : PostProcessEffect("ToneMapping", true)
    {
        AIKO_ASSERT(shaderId != InvalidAssetId, "Tone mapping requires a valid shader asset");

        m_material.m_shaderId = shaderId;
        m_material.m_lit = false;
        m_material.m_useVertexColor = false;

        m_material.m_renderState.depthTest = false;
        m_material.m_renderState.depthWrite = false;
        m_material.m_renderState.blend = false;
        m_material.m_renderState.cullMode = CullMode::None;
    }

    PostProcessDomain ToneMappingEffect::inputDomain() const
    {
        return PostProcessDomain::HDR;
    }

    PostProcessDomain ToneMappingEffect::outputDomain() const
    {
        return PostProcessDomain::LDR;
    }

    void ToneMappingEffect::execute(PostProcessContext& context)
    {
        m_material.setFloat("u_exposure", m_exposure);
        context.renderFullscreen(m_material);
    }

    void ToneMappingEffect::setExposure(float exposure)
    {
        AIKO_ASSERT(std::isfinite(exposure) && exposure > 0.0f, "Exposure must be finite and greater than zero");
        m_exposure = exposure;
    }

    float ToneMappingEffect::exposure() const
    {
        return m_exposure;
    }
}
