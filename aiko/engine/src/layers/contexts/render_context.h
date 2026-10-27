#pragma once

#include <aiko_types.h>
#include <math/math.h>
#include <types/color.h>
#include <imgui/aiko_imgui.h>

namespace aiko
{

    class SystemConnector;
    class RenderSystem;
    class Mesh;
    class Material;
    struct Transform;
    struct InstanceData;
    struct GpuVertexDrawDesc;
    class Camera;
    class RenderTarget;
    struct LightData;
    class Font;
    struct GpuInstanceDrawDesc;
    struct GpuBillboardDrawDesc;
    class PostProcessSystem;
    class PostProcessPipeline;

    class RenderContext
    {
    public:
        void drawPoint(const vec3& pos, const Color& color);
        void drawLine(const vec3& start, const vec3& end, const Color& color);
        void drawRectangle(const vec3& position, const vec3& size);
        void drawCircle(const vec3& pos, const vec3& size, const uint segments, const Color& color);

        void drawMesh(const Transform& transform, const Mesh& mesh, const Material& material);
        void drawMeshInstanced(const Mesh& mesh, const Material& material, const InstanceData* instances, u32 instanceCount);
        void drawVerticesGpu(const GpuVertexDrawDesc& desc);
        void drawMeshInstancedGpu(const GpuInstanceDrawDesc& desc);
        void drawBillboards(const GpuBillboardDrawDesc& desc);
        void drawFullscreen(const Material& material);
        ivec2 getRenderSize() const;
        void renderToTarget(const Camera& camera, RenderTarget& target);
        ImguiTextureId getTextureId(const AssetId& textureId) const;
        ImguiTextureId getTextureId(const AssetId& textureId, const SamplerState& sampler) const;
        void drawText(const Font& font, string_view text, const Transform& transform, float fontSize, Color color = WHITE);

        void submitLight(const LightData& light);

        PostProcessPipeline& postProcessing() const;

    private:
        friend class LayerContext;

        explicit RenderContext(SystemConnector& connector);

        RenderSystem* m_renderSystem = nullptr;
        PostProcessSystem* m_postProcessSystem = nullptr;
    };
}
