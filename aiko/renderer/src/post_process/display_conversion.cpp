#include "display_conversion.h"

#include "post_process_context.h"

namespace aiko
{
    DisplayConversion::DisplayConversion(AssetId shaderId)
    {
        AIKO_ASSERT(shaderId != InvalidAssetId, "Display conversion requires a shader");

        m_material.m_shaderId = shaderId;
        m_material.m_lit = false;
        m_material.m_useVertexColor = false;
        m_material.m_renderState.depthTest = false;
        m_material.m_renderState.depthWrite = false;
        m_material.m_renderState.blend = false;
        m_material.m_renderState.cullMode = CullMode::None;
    }

    void DisplayConversion::execute(PostProcessContext& context)
    {
        m_material.setFloat("u_exposure", 1.0f);
        context.renderFullscreen(m_material);
    }
}
