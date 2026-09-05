#pragma once

#include <aiko_types.h>
#include <math/math.h>

namespace aiko
{

    class SystemConnector;
    class RenderSystem;
    class Mesh;
    class Material;
    struct Transform;
    struct InstanceData;
    struct GpuVertexDrawDesc;
    class RenderTarget;

    class RenderContext
    {
    public:
        void drawRectangle(const vec3& position, const vec3& size);
        void drawMesh(const Transform& transform, const Mesh& mesh, const Material& material);
        void drawMeshInstanced(const Mesh& mesh, const Material& material, const InstanceData* instances, u32 instanceCount);
        void drawVerticesGpu(const GpuVertexDrawDesc& desc);

        void setSceneRenderTarget(RenderTarget* target);

    private:
        friend class LayerContext;

        explicit RenderContext(SystemConnector& connector);

        RenderSystem* m_renderSystem = nullptr;
    };
}
