#pragma once

#include <aiko_types.h>
#include <math/math.h>
#include <types/color.h>

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
    class Font;
    struct GpuInstanceDrawDesc;

    class RenderContext
    {
    public:
        void drawRectangle(const vec3& position, const vec3& size);
        void drawMesh(const Transform& transform, const Mesh& mesh, const Material& material);
        void drawMeshInstanced(const Mesh& mesh, const Material& material, const InstanceData* instances, u32 instanceCount);
        void drawVerticesGpu(const GpuVertexDrawDesc& desc);
        void drawMeshInstancedGpu(const GpuInstanceDrawDesc& desc);
        void renderToTarget(const Camera& camera, RenderTarget& target);
        ImguiTextureId getTextureId(const AssetId& textureId) const;
        ImguiTextureId getTextureId(const AssetId& textureId, const SamplerState& sampler) const;
        void drawText(const Font& font, string_view text, const Transform& transform, float fontSize, Color color = WHITE);

    private:
        friend class LayerContext;

        explicit RenderContext(SystemConnector& connector);

        RenderSystem* m_renderSystem = nullptr;
    };
}
