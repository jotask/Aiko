#include "render_context.h"

#include "systems/render_system.h"
#include "systems/system_connector.h"
#include "systems/post_process_system.h"

namespace aiko
{

    RenderContext::RenderContext(SystemConnector& connector)
    {
        m_renderSystem = connector.find<RenderSystem>();
        AIKO_ASSERT(m_renderSystem != nullptr, "Required system RenderSystem not found");
        m_postProcessSystem = connector.find<PostProcessSystem>();
        AIKO_ASSERT(m_postProcessSystem != nullptr, "PostProcessSystem not found");
    }

    void RenderContext::drawPoint(const vec3& pos, const Color& color)
    {
        m_renderSystem->renderPoint(pos, color);
    }

    void RenderContext::drawLine(const vec3& start, const vec3& end, const Color& color)
    {
        m_renderSystem->renderLine(start, end, color);
    }

    void RenderContext::drawRectangle(const vec3& position, const vec3& size)
    {
        m_renderSystem->renderRectangle(position, size);
    }

    void RenderContext::drawCircle(const vec3& pos, const vec3& size, const uint segments, const Color& color)
    {
        m_renderSystem->renderCircle(pos, size, segments, color);
    }

    void RenderContext::drawMesh(const Transform& transform, const Mesh& mesh, const Material& material)
    {
        m_renderSystem->render(transform, mesh, material);
    }

    void RenderContext::drawMeshInstanced(const Mesh& mesh, const Material& material, const InstanceData* instances, u32 instanceCount)
    {
        m_renderSystem->renderInstanced(mesh, material, instances, instanceCount);
    }

    void RenderContext::drawVerticesGpu(const GpuVertexDrawDesc& desc)
    {
        m_renderSystem->drawVerticesGpu(desc);
    }

    void RenderContext::drawFullscreen(const Material& material)
    {
        m_renderSystem->drawFullscreen(material);
    }

    ivec2 RenderContext::getRenderSize() const
    {
        return m_renderSystem->getRenderSize();
    }

    void RenderContext::drawMeshInstancedGpu(const GpuInstanceDrawDesc& desc)
    {
        m_renderSystem->drawMeshInstancedGpu(desc);
    }

    void RenderContext::drawBillboards(const GpuBillboardDrawDesc& desc)
    {
        m_renderSystem->drawBillboards(desc);
    }

    void RenderContext::renderToTarget(const Camera& camera, RenderTarget& target)
    {
        m_renderSystem->renderToTarget(camera, target);
    }

    ImguiTextureId RenderContext::getTextureId(const AssetId& textureId) const
    {
        return m_renderSystem->getTextureId(textureId);
    }

    ImguiTextureId RenderContext::getTextureId(const AssetId& textureId, const SamplerState& sampler) const
    {
        return m_renderSystem->getTextureId(textureId, sampler);
    }

    void RenderContext::drawText(const Font& font, string_view text, const Transform& transform, float fontSize, Color color)
    {
        m_renderSystem->drawText(font, text, transform, fontSize, color);
    }

    void RenderContext::submitLight(const LightData& light)
    {
        m_renderSystem->submitLight(light);
    }

    PostProcessPipeline& RenderContext::postProcessing() const
    {
        AIKO_ASSERT(m_postProcessSystem != nullptr, "PostProcessSystem not initialized");
        return m_postProcessSystem->pipeline();
    }

}
